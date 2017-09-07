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
#include <exception>

namespace sr
{

    template<class Deleter>
    class scope_success
    {
    public:

        template<class D,
            std::enable_if_t<std::is_constructible<Deleter, D>::value, int> = 0,
            std::enable_if_t<(!std::is_lvalue_reference<D>::value)
                                && std::is_nothrow_constructible<Deleter, D>::value, int> = 0
            >
        explicit scope_success(D&& deleter) : m_deleter(std::move(deleter)),
                                            m_execute_on_destruction(true)
        {
        }

        template<class D,
            std::enable_if_t<std::is_constructible<Deleter, D>::value, int> = 0,
            std::enable_if_t<std::is_lvalue_reference<D>::value, int> = 0
            >
        explicit scope_success(D&& deleter) try : m_deleter(deleter),
                                            m_execute_on_destruction(true)
        {
        }
        catch( ... )
        {
            throw;
        }

        scope_success(const scope_success&) = delete;

        template<class T = Deleter,
            std::enable_if_t<std::is_nothrow_move_constructible<T>::value, int> = 0
            >
        scope_success(scope_success&& other) : m_deleter(std::move(other.m_deleter)),
                                        m_execute_on_destruction(other.m_execute_on_destruction),
                                        m_uncaught_on_creation(uncaught_exceptions())
        {
            other.release();
        }

        template<class T = Deleter,
            std::enable_if_t<!std::is_nothrow_move_constructible<T>::value, int> = 0
            >
        scope_success(scope_success&& other) : m_deleter(other.m_deleter),
                                        m_execute_on_destruction(other.m_execute_on_destruction)
        {
            other.release();
        }

        ~scope_success() noexcept(noexcept(std::declval<Deleter>()))
        {
            if( (m_execute_on_destruction == true) && ( uncaught_exceptions() <= m_uncaught_on_creation ) )
            {
                m_deleter();
            }
        }


        void release() noexcept
        {
            m_execute_on_destruction = false;
        }


        scope_success& operator=(const scope_success&) = delete;
        scope_success& operator=(scope_success&&) = delete;


    private:

        int uncaught_exceptions() const noexcept
        {
            return ( std::uncaught_exception() == true ? 1 : 0 );
        }

        Deleter m_deleter;
        bool m_execute_on_destruction;
        int m_uncaught_on_creation;
    };


    template<class Deleter>
    scope_success<std::decay_t<Deleter>> make_scope_success(Deleter&& deleter)
    {
        return scope_success<std::decay_t<Deleter>>{std::forward<Deleter>(deleter)};
    }

}

