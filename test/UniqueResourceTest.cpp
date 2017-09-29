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

#include "unique_resource.h"
#include <catch.hpp>
#include <trompeloeil.hpp>

using namespace trompeloeil;

namespace
{
    using Handle = int;
    using PtrHandle = std::add_pointer_t<Handle>;


    struct CallMock
    {
        MAKE_MOCK1(deleter, void(Handle));
    };


    struct ThrowOnCopyMock
    {
        ThrowOnCopyMock() {  }

        ThrowOnCopyMock(const ThrowOnCopyMock&)
        {
            throw std::exception{};
        }

        MAKE_CONST_MOCK1(deleter, void(ThrowOnCopyMock));

        ThrowOnCopyMock& operator=(const ThrowOnCopyMock&)
        {
            throw std::exception{};
        }
    };

    struct NotNothrowMoveMock
    {
        NotNothrowMoveMock(CallMock* mo) : m_mock(mo) { }
        NotNothrowMoveMock(const NotNothrowMoveMock& other) : m_mock(other.m_mock)  { }
        NotNothrowMoveMock(NotNothrowMoveMock&& other) noexcept(false) : m_mock(other.m_mock) { }

        void operator()(Handle h) const
        {
            m_mock->deleter(h);
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
        explicit ConditialThrowOnCopyMock(Handle h, bool shouldThrow) : m_handle(h), m_shouldThrow(shouldThrow)
        {
        }

        ConditialThrowOnCopyMock(const ConditialThrowOnCopyMock& other) : m_handle(other.m_handle), m_shouldThrow(other.m_shouldThrow)
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


    CallMock m;

    void deleter(Handle h)
    {
        m.deleter(h);
    }

}

TEST_CASE("construction with move", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    auto guard = sr::make_unique_resource(Handle{3}, deleter);
    static_cast<void>(guard);
}

TEST_CASE("construction with copy", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    const Handle h{3};
    const auto d = [](auto v) { m.deleter(v); };
    auto guard = sr::make_unique_resource(h, d);
    static_cast<void>(guard);
}

TEST_CASE("construction with copy calls deleter and rethrows on failed copy", "[UniqueResource]")
{
    REQUIRE_THROWS([] {
        const ThrowOnCopyMock noMove;
        const auto d = [](const auto&) { m.deleter(3); };
        REQUIRE_CALL(m, deleter(3));

        sr::unique_resource<decltype(noMove), decltype(d)> guard{noMove, d};
        static_cast<void>(guard);
    }());
}

TEST_CASE("move-construction with move", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    auto movedFrom = sr::make_unique_resource(Handle{3}, deleter);
    auto guard = std::move(movedFrom);
    CHECK(guard.get() == 3);
}

TEST_CASE("move-construction with copy", "[UniqueResource]")
{
    CallMock mock;
    REQUIRE_CALL(mock, deleter(3));
    const NotNothrowMoveMock notNothrow{&mock};
    Handle h{3};
    sr::unique_resource<Handle, decltype(notNothrow)> movedFrom{h, notNothrow};
    auto guard = std::move(movedFrom);
    CHECK(guard.get() == 3);
}

TEST_CASE("move assignment calls deleter", "[UniqueResource]")
{
    auto moveFrom = sr::make_unique_resource(Handle{3}, deleter);
    REQUIRE_CALL(m, deleter(4));

    {
        REQUIRE_CALL(m, deleter(3));
        auto guard = sr::make_unique_resource(Handle{4}, deleter);
        guard = std::move(moveFrom);
    }
}

TEST_CASE("deleter called on destruction", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    auto guard = sr::make_unique_resource(Handle{3}, deleter);
    static_cast<void>(guard);
}

TEST_CASE("reset calls deleter", "[UniqueResource]")
{
    auto guard = sr::make_unique_resource(Handle{3}, deleter);

    {
        REQUIRE_CALL(m, deleter(3));
        guard.reset();
    }
}

TEST_CASE("reset does not call deleter if released", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3)).TIMES(0);
    auto guard = sr::make_unique_resource(Handle{3}, deleter);
    guard.release();
    guard.reset();
}

TEST_CASE("reset sets new value and calls deleter on previous", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    REQUIRE_CALL(m, deleter(7));
    auto guard = sr::make_unique_resource(Handle{3}, deleter);
    guard.reset(Handle{7});
}

TEST_CASE("release disables deleter", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3)).TIMES(0);
    auto guard = sr::make_unique_resource(Handle{3}, deleter);
    guard.release();
}

TEST_CASE("get returns resource", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    auto guard = sr::make_unique_resource(Handle{3}, deleter);
    CHECK(guard.get() == 3);
}

TEST_CASE("pointer access returns resource" "[UniqueResource]")
{
    const auto p = std::make_pair(3, 4);
    auto guard = sr::make_unique_resource(&p, [](auto*) { });
    REQUIRE(guard->first == 3);
    REQUIRE(guard->second == 4);
}

TEST_CASE("pointer dereference returns resource" "[UniqueResource]")
{
    Handle h{5};
    auto guard = sr::make_unique_resource(PtrHandle{&h}, [](auto*) { });
    REQUIRE(*guard == 5);
}

TEST_CASE("deleter access", "[UniqueResource]")
{
    auto guard = sr::make_unique_resource(Handle{3}, deleter);
    guard.release();

    {
        REQUIRE_CALL(m, deleter(8));
        guard.get_deleter()(8);
    }
}

TEST_CASE("make unique resource", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(7));
    auto guard = sr::make_unique_resource(Handle{7}, deleter);
    static_cast<void>(guard);
}

