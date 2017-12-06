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

#include <trompeloeil.hpp>

namespace mock
{
    using Handle = int;
    using PtrHandle = std::add_pointer_t<Handle>;


    struct CallMock
    {
        MAKE_MOCK0(deleter, void());
        MAKE_MOCK1(deleter, void(Handle));
    };


    struct ThrowOnCopyMock
    {
        ThrowOnCopyMock()
        {
        }

        ThrowOnCopyMock(const ThrowOnCopyMock&)
        {
            throw std::exception{};
        }


        MAKE_CONST_MOCK0(deleter, void());

        void operator()() const
        {
            this->deleter();
        }

        ThrowOnCopyMock& operator=(const ThrowOnCopyMock&)
        {
            throw std::exception{};
        }
    };


    struct NotNothrowMoveMock
    {
        explicit NotNothrowMoveMock(CallMock* m) : m_mock(m)
        {
        }

        NotNothrowMoveMock(const NotNothrowMoveMock& other) : m_mock(other.m_mock)
        {
        }

        NotNothrowMoveMock(NotNothrowMoveMock&& other) noexcept(false) : m_mock(other.m_mock)
        {
        }


        void operator()() const
        {
            m_mock->deleter();
        }

        NotNothrowMoveMock& operator=(const NotNothrowMoveMock&)
        {
            throw "Not implemented";
        }

        NotNothrowMoveMock& operator=(NotNothrowMoveMock&&)
        {
            throw "Not implemented";
        }


        CallMock* m_mock;
    };


    struct ConditialThrowOnCopyMock
    {
        explicit ConditialThrowOnCopyMock(Handle h, bool shouldThrow) : m_handle(h),
                                                                    m_shouldThrow(shouldThrow)
        {
        }

        ConditialThrowOnCopyMock(const ConditialThrowOnCopyMock& other) : m_handle(other.m_handle),
                                                                    m_shouldThrow(other.m_shouldThrow)
        {
            if( m_shouldThrow == true )
            {
                throw std::exception{};
            }
        }

        ConditialThrowOnCopyMock(ConditialThrowOnCopyMock&&) = default;

        ConditialThrowOnCopyMock& operator=(const ConditialThrowOnCopyMock& other)
        {
            if( &other != this )
            {
                m_handle = other.m_handle;
                m_shouldThrow = other.m_shouldThrow;

                if( m_shouldThrow == true )
                {
                    throw std::exception{};
                }
            }

            return *this;
        }

        ConditialThrowOnCopyMock& operator=(ConditialThrowOnCopyMock&&) = default;


        Handle m_handle;
        bool m_shouldThrow;
    };


    struct NotNothrowAssignable
    {
        explicit NotNothrowAssignable(int value) : m_value(value) { }
        NotNothrowAssignable(const NotNothrowAssignable&) = default;

        NotNothrowAssignable& operator=(const NotNothrowAssignable& other)
        {
            if( this != &other )
            {
                assignNotNoexcept(other.m_value);
            }
            return *this;
        }

        void assignNotNoexcept(int value) noexcept(false)
        {
            m_value = value;
        }

        int m_value;
    };

    struct CopyMock
    {
        CopyMock() { }
        CopyMock(const CopyMock&) { }
    };

}

