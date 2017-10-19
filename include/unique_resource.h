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
#include "detail/wrapper.h"
#include <utility>
#include <type_traits>

namespace sr
{
    namespace detail
    {
        template<class T, class TT>
        using is_ntmocp_constructible = std::conditional_t<std::is_reference_v<TT>
                                                            || !std::is_nothrow_move_constructible_v<TT>,
                                                        typename std::is_constructible<T, const TT&>::type,
                                                        typename std::is_constructible<T, TT>::type>;

        template<class T, class TT>
        constexpr auto is_nothrow_move_or_copy_constructible_from_v = is_ntmocp_constructible<T, TT>::value;


        template<class T>
        constexpr auto is_nothrow_swappable_v = std::is_nothrow_move_constructible_v<T>
                                                && std::is_nothrow_move_assignable_v<T>;



        template<class T, class U = std::conditional_t<std::is_nothrow_move_constructible_v<T>, T&&, const T&>>
        constexpr U forward_if_nothrow_move_constructible(T&& value) noexcept
        {
            return std::forward<T>(value);
        }


    }


    template<class R, class D>
    class unique_resource
    {
    public:

        template<class RR, class DD,
                std::enable_if_t<(std::is_copy_constructible_v<R> || std::is_nothrow_move_constructible_v<R>)
                                && (std::is_copy_constructible_v<D> || std::is_nothrow_move_constructible_v<D>), int> = 0,
                std::enable_if_t<detail::is_nothrow_move_or_copy_constructible_from_v<R, RR>, int> = 0,
                std::enable_if_t<detail::is_nothrow_move_or_copy_constructible_from_v<D, DD>, int> = 0
                >
        explicit unique_resource(RR&& r, DD&& d) noexcept(std::is_nothrow_constructible_v<R, RR>
                                                            && std::is_nothrow_constructible_v<D, DD>)
                                                : m_resource(std::forward<RR>(r), scope_exit{[&r, &d] { d(r); }}),
                                                m_deleter(std::forward<DD>(d), scope_exit{[this, &d] { d(get()); }}),
                                                m_execute_on_destruction(true)
        {
        }

        unique_resource(unique_resource&& other) noexcept(std::is_nothrow_move_constructible_v<R>
                                                            && std::is_nothrow_move_constructible_v<D>)
                                                : m_resource(detail::forward_if_nothrow_move_constructible(other.m_resource.get()), scope_exit{[] { }}),
                                                m_deleter(detail::forward_if_nothrow_move_constructible(other.m_deleter.get()),
                                                                                                        scope_exit{[&other] {
                                                                                                            other.get_deleter()(other.m_resource.get());
                                                                                                            other.release(); }}),
                                                m_execute_on_destruction(std::exchange(other.m_execute_on_destruction, false))
        {
        }

        unique_resource(const unique_resource&) = delete;

        ~unique_resource()
        {
            reset();
        }


        void swap(unique_resource& other) noexcept(detail::is_nothrow_swappable_v<R>
                                                    && detail::is_nothrow_swappable_v<D>
                                                    && detail::is_nothrow_swappable_v<bool>)
        {
            using std::swap;
            swap(m_resource.get(), other.m_resource.get());
            swap(m_deleter.get(), other.m_deleter.get());
            swap(m_execute_on_destruction, other.m_execute_on_destruction);
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
            auto se = scope_exit{[this, &r] { get_deleter()(r); }};
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
                std::enable_if_t<std::is_pointer_v<RR>
                                && (std::is_class_v<std::remove_pointer_t<RR>>
                                    || std::is_union_v<std::remove_pointer_t<RR>>), int> = 0
                >
        RR operator->() const noexcept
        {
            return m_resource.get();
        }

        template<class RR = R,
            std::enable_if_t<( std::is_pointer_v<RR>
                            && !std::is_void_v<std::remove_pointer_t<RR>>), int> = 0>
        std::add_lvalue_reference_t<std::remove_pointer_t<RR>> operator*() const noexcept
        {
            return *get();
        }

        const D& get_deleter() const noexcept
        {
            return m_deleter.get();
        }


        template<class RR = R, class DD = D,
            std::enable_if_t<(std::is_nothrow_move_assignable_v<RR>
                                || std::is_nothrow_copy_assignable_v<RR>)
                            && (std::is_nothrow_move_assignable_v<DD>
                                || std::is_nothrow_copy_assignable_v<DD>), int> = 0
            >
        unique_resource& operator=(unique_resource&& other)
        {
            if( this != &other )
            {
                reset();

                if constexpr( std::is_nothrow_move_assignable_v<R> == true )
                {
                    m_deleter.reset(detail::forward_if_nothrow_move_constructible(other.m_deleter.get()));
                    m_resource.reset(std::forward<RR>(other.m_resource.get()));
                }
                else if constexpr( std::is_nothrow_move_assignable_v<D> == true )
                {
                    m_resource.reset(detail::forward_if_nothrow_move_constructible(other.m_resource.get()));
                    m_deleter.reset(std::forward<DD>(other.m_deleter.get()));
                }
                else
                {
                    m_resource = other.m_resource;
                    m_deleter = other.m_deleter;
                }

                m_execute_on_destruction = std::exchange(other.m_execute_on_destruction, false);
            }
            return *this;
        }

        unique_resource& operator=(const unique_resource&) = delete;


    private:

        detail::Wrapper<R> m_resource;
        detail::Wrapper<D> m_deleter;
        bool m_execute_on_destruction;
    };


    template<class R, class D>
    unique_resource(R&& r, D&& d) -> unique_resource<std::decay_t<R>, std::decay_t<D>>;

    template<class R, class D>
    unique_resource(std::reference_wrapper<R> r, D&& d) -> unique_resource<R&, std::decay_t<D>>;


    template<class R, class D, class S = R>
    unique_resource<std::decay_t<R>, std::decay_t<D>> make_unique_resource_checked(R&& r, const S& invalid, D&& d)
                                                            noexcept(std::is_nothrow_constructible_v<std::decay_t<R>, R>
                                                                    && std::is_nothrow_constructible_v<std::decay_t<D>, D>)
    {
        const bool must_release{r == invalid};
        auto ur = unique_resource{r, d};

        if( must_release == true )
        {
            ur.release();
        }

        return ur;
    }

}


