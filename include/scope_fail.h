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

    template<class EF>
    class scope_fail
    {
    public:

        template<class EFP,
            std::enable_if_t<std::is_constructible<EF, EFP>::value, int> = 0,
            std::enable_if_t<(!std::is_lvalue_reference<EFP>::value)
                                && std::is_nothrow_constructible<EF, EFP>::value, int> = 0
            >
        explicit scope_fail(EFP&& exitFunction) : m_exitFunction(std::move(exitFunction)),
                                            m_execute_on_destruction(true),
                                            m_uncaught_on_creation(uncaught_exceptions())
        {
        }

        template<class EFP,
            std::enable_if_t<std::is_constructible<EF, EFP>::value, int> = 0,
            std::enable_if_t<std::is_lvalue_reference<EFP>::value, int> = 0
            >
        explicit scope_fail(EFP&& exitFunction) try : m_exitFunction(exitFunction),
                                            m_execute_on_destruction(true),
                                            m_uncaught_on_creation(uncaught_exceptions())
        {
        }
        catch( ... )
        {
            exitFunction();
            throw;
        }

        scope_fail(const scope_fail&) = delete;

        template<class T = EF,
            std::enable_if_t<std::is_nothrow_move_constructible<T>::value, int> = 0
            >
        scope_fail(scope_fail&& other) noexcept(std::is_nothrow_move_constructible<T>::value || std::is_nothrow_copy_constructible<T>::value)
                                        : m_exitFunction(std::move(other.m_exitFunction)),
                                        m_execute_on_destruction(other.m_execute_on_destruction),
                                        m_uncaught_on_creation(uncaught_exceptions())
        {
            other.release();
        }

        template<class T = EF,
            std::enable_if_t<!std::is_nothrow_move_constructible<T>::value, int> = 0
            >
        scope_fail(scope_fail&& other) noexcept(std::is_nothrow_move_constructible<T>::value || std::is_nothrow_copy_constructible<T>::value)
                                        : m_exitFunction(other.m_exitFunction),
                                        m_execute_on_destruction(other.m_execute_on_destruction),
                                        m_uncaught_on_creation(other.m_uncaught_on_creation)
        {
            other.release();
        }

        ~scope_fail() noexcept(noexcept(std::declval<EF>()))
        {
            if( ( m_execute_on_destruction == true ) && ( uncaught_exceptions() > m_uncaught_on_creation ) )
            {
                m_exitFunction();
            }
        }


        void release() noexcept
        {
            m_execute_on_destruction = false;
        }


        scope_fail& operator=(const scope_fail&) = delete;
        scope_fail& operator=(scope_fail&&) = delete;


    private:

        int uncaught_exceptions() const noexcept
        {
            return ( std::uncaught_exception() == true ? 1 : 0 );
        }

        EF m_exitFunction;
        bool m_execute_on_destruction;
        int m_uncaught_on_creation;
    };


    template<class EF>
    scope_fail<std::decay_t<EF>> make_scope_fail(EF&& exitFunction)
    {
        return scope_fail<std::decay_t<EF>>{std::forward<EF>(exitFunction)};
    }

}

