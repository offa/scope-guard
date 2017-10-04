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

    template<class Deleter>
    class scope_exit
    {
    public:

        template<class D,
            std::enable_if_t<std::is_constructible<Deleter, D>::value, int> = 0,
            std::enable_if_t<(!std::is_lvalue_reference<D>::value)
                                && std::is_nothrow_constructible<Deleter, D>::value, int> = 0
            >
        explicit scope_exit(D&& deleter) : m_exitFunction(std::move(deleter)),
                                            m_execute_on_destruction(true)
        {
        }

        template<class D,
            std::enable_if_t<std::is_constructible<Deleter, D>::value, int> = 0,
            std::enable_if_t<std::is_lvalue_reference<D>::value, int> = 0
            >
        explicit scope_exit(D&& deleter) try : m_exitFunction(deleter),
                                            m_execute_on_destruction(true)
        {
        }
        catch( ... )
        {
            deleter();
            throw;
        }

        scope_exit(const scope_exit&) = delete;

        template<class T = Deleter,
            std::enable_if_t<std::is_nothrow_move_constructible<T>::value, int> = 0
            >
        scope_exit(scope_exit&& other) : m_exitFunction(std::move(other.m_exitFunction)),
                                        m_execute_on_destruction(other.m_execute_on_destruction)
        {
            other.release();
        }

        template<class T = Deleter,
            std::enable_if_t<!std::is_nothrow_move_constructible<T>::value, int> = 0
            >
        scope_exit(scope_exit&& other) : m_exitFunction(other.m_exitFunction),
                                        m_execute_on_destruction(other.m_execute_on_destruction)
        {
            other.release();
        }

        ~scope_exit() noexcept(true)
        {
            if( m_execute_on_destruction == true )
            {
                m_exitFunction();
            }
        }


        void release() noexcept
        {
            m_execute_on_destruction = false;
        }


        scope_exit& operator=(const scope_exit&) = delete;
        scope_exit& operator=(scope_exit&&) = delete;


    private:

        Deleter m_exitFunction;
        bool m_execute_on_destruction;
    };


    template<class Deleter>
    scope_exit<std::decay_t<Deleter>> make_scope_exit(Deleter&& deleter)
    {
        return scope_exit<std::decay_t<Deleter>>{std::forward<Deleter>(deleter)};
    }

}

