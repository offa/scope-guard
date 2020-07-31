// MIT License
//
// Copyright (c) 2017-2020 offa
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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


    struct MoveableMock
    {
        static constexpr bool trompeloeil_movable_mock = true;

        MAKE_CONST_MOCK1(deleter, void(Handle));

        void operator()(Handle h) const
        {
            this->deleter(h);
        }
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
        explicit NotNothrowMoveMock(CallMock* m) : mock(m)
        {
        }

        NotNothrowMoveMock(const NotNothrowMoveMock& other) : mock(other.mock)
        {
        }

        NotNothrowMoveMock(NotNothrowMoveMock&& other) noexcept(false) : mock(other.mock)
        {
        }


        void operator()() const
        {
            mock->deleter();
        }

        NotNothrowMoveMock& operator=(const NotNothrowMoveMock&)
        {
            throw "Not implemented";
        }

        NotNothrowMoveMock& operator=(NotNothrowMoveMock&&)
        {
            throw "Not implemented";
        }


        CallMock* mock;
    };


    struct ConditionalThrowOnCopyMock
    {
        explicit ConditionalThrowOnCopyMock(Handle h, bool throwOnCopyMock) : handle(h),
                                                                    shouldThrow(throwOnCopyMock)
        {
        }

        ConditionalThrowOnCopyMock(const ConditionalThrowOnCopyMock& other) : handle(other.handle),
                                                                    shouldThrow(other.shouldThrow)
        {
            if( shouldThrow == true )
            {
                throw std::exception{};
            }
        }

        ConditionalThrowOnCopyMock(ConditionalThrowOnCopyMock&&) = default;

        ConditionalThrowOnCopyMock& operator=(const ConditionalThrowOnCopyMock& other)
        {
            if( &other != this )
            {
                handle = other.handle;
                shouldThrow = other.shouldThrow;

                if( shouldThrow == true )
                {
                    throw std::exception{};
                }
            }

            return *this;
        }

        ConditionalThrowOnCopyMock& operator=(ConditionalThrowOnCopyMock&&) = default;


        Handle handle;
        bool shouldThrow;
    };


    struct NotNothrowAssignable
    {
        explicit NotNothrowAssignable(int v) : value(v) { }
        NotNothrowAssignable(const NotNothrowAssignable&) = default;

        NotNothrowAssignable& operator=(const NotNothrowAssignable& other)
        {
            if( this != &other )
            {
                assignNotNoexcept(other.value);
            }
            return *this;
        }

        void assignNotNoexcept(int v) noexcept(false)
        {
            value = v;
        }

        int value;
    };

    struct CopyMock
    {
        CopyMock() { }
        CopyMock(const CopyMock&) { }
    };

    struct ConditionalThrowOnCopyDeleter
    {
        ConditionalThrowOnCopyDeleter() { }

        ConditionalThrowOnCopyDeleter(const ConditionalThrowOnCopyDeleter&)
        {
            if( throwOnNextCopy == true )
            {
                throw std::exception{};
            }
            throwOnNextCopy = false;
        }

        MAKE_CONST_MOCK1(deleter, void(Handle));

        void operator()(Handle h) const
        {
            this->deleter(h);
        }

        static inline bool throwOnNextCopy{false};
    };

    template<bool noexceptMove>
    struct NoexceptResource
    {
        NoexceptResource() = default;
        ~NoexceptResource() = default;

        NoexceptResource(const NoexceptResource&) = default;

        NoexceptResource(NoexceptResource&&) noexcept(noexceptMove)
        {
        }

        NoexceptResource& operator=(const NoexceptResource&) = default;

        NoexceptResource& operator=(NoexceptResource&&) noexcept(noexceptMove)
        {
            return *this;
        }
    };

    template<bool noexceptMove>
    struct NoexceptDeleter
    {
        NoexceptDeleter() = default;
        ~NoexceptDeleter() = default;

        NoexceptDeleter(const NoexceptDeleter&) = default;

        NoexceptDeleter(NoexceptDeleter&&) noexcept(noexceptMove)
        {
        }

        NoexceptDeleter& operator=(const NoexceptDeleter&) = default;

        NoexceptDeleter& operator=(NoexceptDeleter&&) noexcept(noexceptMove)
        {
            return *this;
        }

        template<class Resource>
        void operator()(const Resource&) const
        {
        }
    };


}
