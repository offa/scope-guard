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
        ThrowOnCopyMock()
        {
        }

        ThrowOnCopyMock(const ThrowOnCopyMock&) noexcept(false)
        {
            throw std::exception{};
        }

        ThrowOnCopyMock(ThrowOnCopyMock&&) = delete;


        ThrowOnCopyMock& operator=(const ThrowOnCopyMock&) noexcept(false)
        {
            throw std::exception{};
        }

        ThrowOnCopyMock& operator=(ThrowOnCopyMock&&) = delete;

    };

    struct NotNothrowMoveMock
    {
        NotNothrowMoveMock(CallMock* mo) : m_mock(mo)
        {
        }

        NotNothrowMoveMock(const NotNothrowMoveMock& other) : m_mock(other.m_mock)
        {
            throw std::exception{};
        }

        NotNothrowMoveMock(NotNothrowMoveMock&& other) noexcept(false) : m_mock(other.m_mock)
        {
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

    struct CopyMock
    {
        CopyMock() {}
        CopyMock(const CopyMock&) { }
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
    REQUIRE_CALL(m, deleter(7));
    auto d = [](auto) { deleter(7); };

    const CopyMock copyMock;
    sr::unique_resource<CopyMock, decltype(d)> movedFrom{copyMock, d};
    auto guard = std::move(movedFrom);
    static_cast<void>(guard);
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

TEST_CASE("reset handles exception on assignment", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    REQUIRE_CALL(m, deleter(7));
    auto d = [](const auto& v) { deleter(v.m_handle); };
    auto guard = sr::make_unique_resource(ConditialThrowOnCopyMock{3, false}, d);
    guard.reset(ConditialThrowOnCopyMock{7, true});
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

TEST_CASE("swap", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(7));
    auto guard1 = sr::make_unique_resource(Handle{3}, deleter);

    {
        REQUIRE_CALL(m, deleter(3));
        auto guard2 = sr::make_unique_resource(Handle{7}, deleter);
        guard2.swap(guard1);
        REQUIRE(guard1.get() == 7);
        REQUIRE(guard2.get() == 3);
    }
}

TEST_CASE("make unique resource", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(7));
    auto guard = sr::make_unique_resource(Handle{7}, deleter);
    static_cast<void>(guard);
}

TEST_CASE("make unique resource with reference wrapper", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    Handle h{3};
    auto guard = sr::make_unique_resource(std::ref(h), deleter);
    static_cast<void>(guard);
}

TEST_CASE("make unique resource checked", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(4));
    auto guard = sr::make_unique_resource_checked(Handle{4}, Handle{-1}, deleter);
    static_cast<void>(guard);
}

TEST_CASE("make unique resource checked releases if invalid", "[UniqueResource]")
{
    auto guard = sr::make_unique_resource_checked(Handle{-1}, Handle{-1}, deleter);
    static_cast<void>(guard);
}

