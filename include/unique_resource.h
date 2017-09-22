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
        explicit unique_resource(RR&& r, DD& d) : m_resource(std::move(r)), m_deleter(std::move(d))
        {
        }

        template<class RR, class DD,
                std::enable_if_t<std::is_lvalue_reference<RR>::value || std::is_lvalue_reference<DD>::value, int> = 0,

                std::enable_if_t<(std::is_copy_constructible<R>::value || std::is_nothrow_move_constructible<R>::value)
                                && (std::is_copy_constructible<D>::value || std::is_nothrow_move_constructible<D>::value), int> = 0,
                std::enable_if_t<is_nothrow_move_or_copy_constructible_from_v<R, RR>, int> = 0,
                std::enable_if_t<is_nothrow_move_or_copy_constructible_from_v<D, DD>, int> = 0
                >
        explicit unique_resource(RR&& r, DD& d) try : m_resource(r), m_deleter(d)
        {
        }
        catch( ... )
        {
            d(r);
            throw;
        }

        template<class TR = R, class TD = D,
                std::enable_if_t<(std::is_nothrow_move_constructible<TR>::value
                                && std::is_nothrow_move_constructible<TD>::value), int> = 0
                >
        unique_resource(unique_resource&& other) : m_resource(std::forward<R>(other.m_resource)),
                                                m_deleter(std::forward<D>(other.m_deleter)),
                                                m_execute_on_destruction(std::exchange(other.m_execute_on_destruction, false))
        {
        }

        template<class TR = R, class TD = D,
                std::enable_if_t<(!std::is_nothrow_move_constructible<TR>::value
                                || !std::is_nothrow_move_constructible<TD>::value), int> = 0
                >
        unique_resource(unique_resource&& other) : m_resource(other.m_resource),
                                                m_deleter(other.m_deleter),
                                                m_execute_on_destruction(std::exchange(other.m_execute_on_destruction, false))
        {
        }

        unique_resource(const unique_resource&) = delete;



        unique_resource& operator=(unique_resource&& other);
        unique_resource& operator=(const unique_resource&) = delete;


    private:

        R m_resource;
        D m_deleter;
        bool m_execute_on_destruction;
    };


    template<class Resource, class Deleter>
    unique_resource<std::decay_t<Resource>, std::decay_t<Deleter>> make_unique_resource(Resource&& r, Deleter&& d)
    {
        return unique_resource<std::decay_t<Resource>, std::decay_t<Deleter>>{std::forward<Resource>(r), std::forward<Deleter>(d)};
    }


}


