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
    class scope_guard_t
    {
    public:

        template<class D,
            std::enable_if_t<std::is_constructible<Deleter, D>::value, int> = 0,
            std::enable_if_t<(!std::is_lvalue_reference<D>::value)
                                && std::is_nothrow_constructible<Deleter, D>::value, int> = 0
            >
        explicit scope_guard_t(D&& deleter) : m_deleter(std::move(deleter)),
                                                        m_execute_on_destruction(true)
        {
        }

        template<class D,
            std::enable_if_t<std::is_constructible<Deleter, D>::value, int> = 0,
            std::enable_if_t<std::is_lvalue_reference<D>::value, int> = 0
            >
        explicit scope_guard_t(D&& deleter) : m_deleter(deleter),
                                                        m_execute_on_destruction(true)
        {
            // TODO: Handle copying correctly (#41)
        }


        scope_guard_t(const scope_guard_t&) = delete;

        scope_guard_t(scope_guard_t&& other) noexcept : m_deleter(std::move(other.m_deleter)),
                                                    m_execute_on_destruction(other.m_execute_on_destruction)
        {
            other.release();
        }

        ~scope_guard_t()
        {
            if( m_execute_on_destruction == true )
            {
                call_deleter_safe();
            }
        }


        void release() noexcept
        {
            m_execute_on_destruction = false;
        }


        scope_guard_t& operator=(const scope_guard_t&) = delete;
        scope_guard_t& operator=(scope_guard_t&&) = delete;


    private:

        void call_deleter_safe() noexcept
        {
            try
            {
                m_deleter();
            }
            catch( ... ) { /* Empty */ }
        }


        Deleter m_deleter;
        bool m_execute_on_destruction;
    };


    template<class Deleter>
    scope_guard_t<Deleter> scope_guard(Deleter&& deleter) noexcept
    {
        return scope_guard_t<Deleter>{std::move(deleter)};
    }

}

