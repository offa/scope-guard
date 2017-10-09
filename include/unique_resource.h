/*
 * Scope Guard
 * Copyright (C) 2017  offa
 *
 * This file is part of Scope Guard.
 *
 * Scope Guard is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Scope Guard is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Scope Guard.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "scope_exit.h"
#include <utility>
#include <type_traits>

namespace sr
{

    template<class T, class TT>
    using is_ntmocp_constructible = std::conditional_t<std::is_reference<TT>::value || !std::is_nothrow_move_constructible<TT>::value,
                                                    typename std::is_constructible<T, const TT&>::type,
                                                    typename std::is_constructible<T, TT>::type>;

    template<class T, class TT>
    constexpr auto is_nothrow_move_or_copy_constructible_from_v = is_ntmocp_constructible<T, TT>::value;


    template<class T,
            class U = std::conditional_t<(!std::is_nothrow_move_assignable<T>::value
                                            && std::is_copy_assignable<T>::value),
                                        const T&,
                                        T &&>>
    constexpr U move_assign_if_noexcept(T& value) noexcept
    {
        return std::move(value);
    }


    template<class T, class U = std::conditional_t<std::is_nothrow_move_constructible<T>::value, T&&, const T&>>
    constexpr U forward_if_nothrow_move_constructible(T&& value)
    {
        return std::forward<T>(value);
    }


    // TODO: Fix old-style casts
    // TODO: make class and fix noexcept's
    template<class T>
    struct Wrapper
    {
        template<class TT, class G, std::enable_if_t<std::is_constructible<T, TT>::value, int> = 0>
        explicit Wrapper(TT&& value, G&& g) : Wrapper((T&&) value)
        {
            g.release();
        }


        T& get()
        {
            return m_value;
        }

        const T& get() const
        {
            return m_value;
        }

        void reset(T&& newValue)
        {
            m_value = move_assign_if_noexcept(newValue);
        }

        void reset(const T& newValue)
        {
            m_value = newValue;
        }



    private:

        Wrapper(const T& value) : m_value(value)
        {
        }

        Wrapper(T&& value) : m_value(std::move_if_noexcept(value))
        {
        }


        T m_value;
    };

    template<class T>
    struct Wrapper<T&>
    {
        template<class TT, class G, std::enable_if_t<std::is_convertible<TT, T&>::value, int> = 0>
        explicit Wrapper(TT&& value, G&& g) : m_value((T&) value)
        {
            g.release();
        }


        T& get()
        {
            return m_value.get();
        }

        const T& get() const
        {
            return m_value.get();
        }

    private:


        std::reference_wrapper<T> m_value;
    };




    template<class R, class D>
    class unique_resource
    {
    public:

        template<class RR, class DD,
                std::enable_if_t<(std::is_copy_constructible<R>::value || std::is_nothrow_move_constructible<R>::value)
                                && (std::is_copy_constructible<D>::value || std::is_nothrow_move_constructible<D>::value), int> = 0,
                std::enable_if_t<is_nothrow_move_or_copy_constructible_from_v<R, RR>, int> = 0,
                std::enable_if_t<is_nothrow_move_or_copy_constructible_from_v<D, DD>, int> = 0
                >
        explicit unique_resource(RR&& r, DD&& d) noexcept(std::is_nothrow_constructible<R, RR>::value
                                                            && std::is_nothrow_constructible<D, DD>::value)
                                                : m_resource(std::forward<RR>(r), make_scope_exit([&r, &d] { d(r); })),
                                                m_deleter(std::forward<DD>(d), make_scope_exit([this, &d] { d(get()); })),
                                                m_execute_on_destruction(true)
        {
        }

        unique_resource(unique_resource&& other) noexcept(std::is_nothrow_move_constructible<R>::value
                                                    && std::is_nothrow_move_constructible<D>::value)
                                                : m_resource(forward_if_nothrow_move_constructible(other.m_resource.get()), make_scope_exit([] { })),
                                                m_deleter(forward_if_nothrow_move_constructible(other.m_deleter.get()), make_scope_exit([&other] {
                                                                                                                            other.get_deleter()(other.m_resource.get());
                                                                                                                            other.release(); })),
                                                m_execute_on_destruction(std::exchange(other.m_execute_on_destruction, false))
        {
        }


        unique_resource(const unique_resource&) = delete;

        ~unique_resource()
        {
            reset();
        }


        void reset()
        {
            if( m_execute_on_destruction == true )
            {
                m_execute_on_destruction = false;
                get_deleter()(m_resource.get());
            }
        }

        template<class RR>
        void reset(RR&& r)
        {
            auto se = make_scope_exit([this, &r] { get_deleter()(r); });
            reset();
            m_resource.reset(std::forward<RR>(r));
            m_execute_on_destruction = true;
            se.release();
        }

        void release()
        {
            m_execute_on_destruction = false;
        }

        decltype(auto) get() const noexcept
        {
            return m_resource.get();
        }

        template<class RR = R,
                std::enable_if_t<std::is_pointer<RR>::value
                                && ( std::is_class<std::remove_pointer_t<RR>>::value
                                    || std::is_union<std::remove_pointer_t<RR>>::value ), int> = 0
                >
        RR operator->() const noexcept
        {
            return m_resource.get();
        }

        template<class RR = R,
            std::enable_if_t<( std::is_pointer<RR>::value && !std::is_void<std::remove_pointer_t<RR>>::value), int> = 0>
        std::add_lvalue_reference_t<std::remove_pointer_t<RR>> operator*() const noexcept
        {
            return *get();
        }

        const D& get_deleter() const noexcept
        {
            return m_deleter.get();
        }


        //template<class RR = R, class DD = D,
            //std::enable_if_t<(std::is_nothrow_move_assignable<RR>::value || std::is_nothrow_copy_assignable<RR>::value)
                            //&& (std::is_nothrow_copy_assignable<DD>::value || std::is_nothrow_copy_assignable<DD>::value), int> = 0
            //>
        //unique_resource& operator=(unique_resource&& other)
        //{
            //if( this != &other )
            //{
                //reset();
                //m_resource = std::forward<RR>(other.m_resource);
                //m_deleter = std::forward<DD>(other.m_deleter);
                //m_execute_on_destruction = std::exchange(other.m_execute_on_destruction, false);
            //}
            //return *this;
        //}
        // FIXME: Needs update
        unique_resource& operator=(unique_resource&&) = default;

        unique_resource& operator=(const unique_resource&) = delete;


    private:

        Wrapper<R> m_resource;
        Wrapper<D> m_deleter;
        bool m_execute_on_destruction;
    };



    template<class R, class D>
    unique_resource<std::decay_t<R>, std::decay_t<D>> make_unique_resource(R&& r, D&& d)
                                                            noexcept(std::is_nothrow_constructible<std::decay_t<R>, R>::value
                                                                    && std::is_nothrow_constructible<std::decay_t<D>, D>::value)
    {
        return unique_resource<std::decay_t<R>, std::decay_t<D>>{std::forward<R>(r), std::forward<D>(d)};
    }

    template<class R, class D>
    unique_resource<R&, std::decay_t<D>> make_unique_resource(std::reference_wrapper<R> r, D d)
                                                noexcept(std::is_nothrow_constructible<std::decay_t<D>, D>::value)
    {
        return unique_resource<R&, std::decay_t<D>>(r.get(), std::forward<D>(d));
    }

    template<class R, class D, class S = R>
    unique_resource<std::decay_t<R>, std::decay_t<D>> make_unique_resource_checked(R&& r, const S& invalid, D&& d)
                                                            noexcept(std::is_nothrow_constructible<std::decay_t<R>, R>::value
                                                                    && std::is_nothrow_constructible<std::decay_t<D>, D>::value)
    {
        const bool mustRelease{r == invalid};
        auto ur = make_unique_resource(r, d);

        if( mustRelease == true )
        {
            ur.release();
        }

        return ur;
    }

}


