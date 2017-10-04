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

#include <utility>
#include <type_traits>

namespace sr
{

    template<class T, class TT>
    using is_ntmocp_constructible = std::conditional_t<std::is_reference<TT>::value || !std::is_nothrow_move_constructible<TT>::value,
                                                    typename std::is_constructible<T, TT const &>::type,
                                                    typename std::is_constructible<T, TT>::type>;

    template<class T, class TT>
    constexpr auto is_nothrow_move_or_copy_constructible_from_v = is_ntmocp_constructible<T, TT>::value;


    template<class T,
            class U = std::conditional_t<(!std::is_nothrow_move_assignable<T>::value
                                            && std::is_copy_assignable<T>::value),
                                        T const &,
                                        T &&>>
    constexpr U move_assign_if_noexcept(T& value) noexcept
    {
        return std::move(value);
    }


    template<class T, std::enable_if_t<std::is_nothrow_move_constructible<T>::value, int> = 0>
    constexpr T&& forward_if_nothrow_move_constructible(T&& value)
    {
        return std::forward<T>(value);
    }

    template<class T, std::enable_if_t<!std::is_nothrow_move_constructible<T>::value, int> = 0>
    constexpr T forward_if_nothrow_move_constructible(T&& value)
    {
        return value;
    }


    template<class R, class D>
    class unique_resource
    {
    public:

        template<class RR, class DD,
                std::enable_if_t<(!std::is_lvalue_reference<RR>::value)
                                    && std::is_nothrow_constructible<R, RR>::value, int> = 0,
                std::enable_if_t<(!std::is_lvalue_reference<DD>::value)
                                    && std::is_nothrow_constructible<D, DD>::value, int> = 0,
                std::enable_if_t<(std::is_copy_constructible<R>::value || std::is_nothrow_move_constructible<R>::value)
                                && (std::is_copy_constructible<D>::value || std::is_nothrow_move_constructible<D>::value), int> = 0,
                std::enable_if_t<is_nothrow_move_or_copy_constructible_from_v<R, RR>, int> = 0,
                std::enable_if_t<is_nothrow_move_or_copy_constructible_from_v<D, DD>, int> = 0
                >
        explicit unique_resource(RR&& r, DD&& d) noexcept(std::is_nothrow_constructible<R,RR>::value
                                                            && std::is_nothrow_constructible<D, DD>::value)
                                                : m_resource(std::move(r)),
                                                m_deleter(std::move(d)),
                                                m_execute_on_destruction(true)
        {
        }

        template<class RR, class DD,
                std::enable_if_t<std::is_lvalue_reference<RR>::value || std::is_lvalue_reference<DD>::value, int> = 0,
                std::enable_if_t<(std::is_copy_constructible<R>::value || std::is_nothrow_move_constructible<R>::value)
                                && (std::is_copy_constructible<D>::value || std::is_nothrow_move_constructible<D>::value), int> = 0,
                std::enable_if_t<is_nothrow_move_or_copy_constructible_from_v<R, RR>, int> = 0,
                std::enable_if_t<is_nothrow_move_or_copy_constructible_from_v<D, DD>, int> = 0
                >
        explicit unique_resource(RR&& r, DD&& d) noexcept(std::is_nothrow_constructible<R, RR>::value
                                                            && std::is_nothrow_constructible<D, DD>::value)
                                                    try : m_resource(r),
                                                        m_deleter(d),
                                                        m_execute_on_destruction(true)
        {
        }
        catch( ... )
        {
            d(r);
        }

        template<class TR = R, class TD = D,
                std::enable_if_t<(std::is_nothrow_move_constructible<TR>::value
                                && std::is_nothrow_move_constructible<TD>::value), int> = 0
                >
        unique_resource(unique_resource&& other) noexcept(std::is_nothrow_move_constructible<R>::value
                                                            && std::is_nothrow_move_constructible<D>::value)
                                                : m_resource(forward_if_nothrow_move_constructible<R>(std::forward<R>(other.m_resource))),
                                                m_deleter(forward_if_nothrow_move_constructible<D>(std::forward<D>(other.m_deleter))),
                                                m_execute_on_destruction(std::exchange(other.m_execute_on_destruction, false))
        {
        }

        template<class TR = R, class TD = D,
                std::enable_if_t<(!std::is_nothrow_move_constructible<TR>::value
                                || !std::is_nothrow_move_constructible<TD>::value), int> = 0
                >
        unique_resource(unique_resource&& other) noexcept(std::is_nothrow_move_constructible<R>::value
                                                            && std::is_nothrow_move_constructible<D>::value)
                                                : m_resource(other.m_resource),
                                                m_deleter(other.m_deleter),
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
                get_deleter()(m_resource);
            }
        }

        template<class RR>
        void reset(RR&& r)
        {
            reset();

            m_resource = move_assign_if_noexcept(r);
            m_execute_on_destruction = true;
        }

        void release()
        {
            m_execute_on_destruction = false;
        }

        const R& get() const noexcept
        {
            return m_resource;
        }

        template<class RR = R,
                std::enable_if_t<std::is_pointer<RR>::value && std::is_nothrow_copy_constructible<RR>::value
                                && ( std::is_class<std::remove_pointer_t<RR>>::value
                                        || std::is_union<std::remove_pointer_t<RR>>::value ), int> = 0
                >
        RR operator->() const noexcept
        {
            return m_resource;
        }

        template<class RR = R,
            std::enable_if_t<std::is_pointer<RR>::value, int> = 0>
        std::add_lvalue_reference_t<std::remove_pointer_t<RR>> operator*() const noexcept
        {
            return *get();
        }

        const D& get_deleter() const noexcept
        {
            return m_deleter;
        }


        template<class RR = R, class DD = D,
            std::enable_if_t<(std::is_nothrow_move_assignable<RR>::value || std::is_nothrow_copy_assignable<RR>::value)
                            && (std::is_nothrow_copy_assignable<DD>::value || std::is_nothrow_copy_assignable<DD>::value), int> = 0
            >
        unique_resource& operator=(unique_resource&& other)
        {
            if( this != &other )
            {
                reset();
                m_resource = std::forward<RR>(other.m_resource);
                m_deleter = std::forward<DD>(other.m_deleter);
                m_execute_on_destruction = std::exchange(other.m_execute_on_destruction, false);
            }
            return *this;
        }

        unique_resource& operator=(const unique_resource&) = delete;


    private:

        R m_resource;
        D m_deleter;
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


