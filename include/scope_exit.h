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

    template<class EF>
    class scope_exit
    {
    public:

        template<class EFP,
            std::enable_if_t<std::is_constructible<EF, EFP>::value, int> = 0,
            std::enable_if_t<(!std::is_lvalue_reference<EFP>::value)
                                && std::is_nothrow_constructible<EF, EFP>::value, int> = 0
            >
        explicit scope_exit(EFP&& exitFunction) : m_exitFunction(std::move(exitFunction)),
                                            m_execute_on_destruction(true)
        {
        }

        template<class EFP,
            std::enable_if_t<std::is_constructible<EF, EFP>::value, int> = 0,
            std::enable_if_t<std::is_lvalue_reference<EFP>::value, int> = 0
            >
        explicit scope_exit(EFP&& exitFunction) try : m_exitFunction(exitFunction),
                                            m_execute_on_destruction(true)
        {
        }
        catch( ... )
        {
            exitFunction();
            throw;
        }

        scope_exit(const scope_exit&) = delete;

        template<class T = EF,
            std::enable_if_t<std::is_nothrow_move_constructible<T>::value, int> = 0
            >
        scope_exit(scope_exit&& other) : m_exitFunction(std::move(other.m_exitFunction)),
                                        m_execute_on_destruction(other.m_execute_on_destruction)
        {
            other.release();
        }

        template<class T = EF,
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

        EF m_exitFunction;
        bool m_execute_on_destruction;
    };


    template<class EF>
    scope_exit<std::decay_t<EF>> make_scope_exit(EF&& exitFunction)
    {
        return scope_exit<std::decay_t<EF>>{std::forward<EF>(exitFunction)};
    }

}

