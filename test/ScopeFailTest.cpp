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

#include "scope_fail.h"
#include <catch.hpp>
#include <trompeloeil.hpp>

using namespace trompeloeil;

namespace
{
    struct CallMock
    {
        MAKE_MOCK0(deleter, void());
    };


    struct ThrowOnCopyMock
    {
        ThrowOnCopyMock() {  }

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
        NotNothrowMoveMock(CallMock* m) : m_mock(m) { }
        NotNothrowMoveMock(const NotNothrowMoveMock& other) : m_mock(other.m_mock)  { }
        NotNothrowMoveMock(NotNothrowMoveMock&& other) noexcept(false) : m_mock(other.m_mock) { }

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


    CallMock m;


    void deleter()
    {
        m.deleter();
    }
}


TEST_CASE("deleter called on destruction", "[ScopeFail]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto guard = sr::make_scope_fail(deleter);
    static_cast<void>(guard);
}

TEST_CASE("deleter lambda called on destruction", "[ScopeFail]")
{
    CallMock cm;
    REQUIRE_CALL(cm, deleter()).TIMES(0);
    auto guard = sr::make_scope_fail([&cm] { cm.deleter(); });
    static_cast<void>(guard);
}

TEST_CASE("deleter called and rethrow on copy exception", "[ScopeFail]")
{
    REQUIRE_THROWS([] {
        const ThrowOnCopyMock noMove;
        REQUIRE_CALL(noMove, deleter());

        sr::scope_fail<decltype(noMove)> guard{noMove};
        }());
}

TEST_CASE("deleter is not called if released", "[ScopeFail]")
{
   REQUIRE_CALL(m, deleter()).TIMES(0);
   auto guard = sr::make_scope_fail(deleter);
   guard.release();
}

TEST_CASE("move releases moved-from object", "[ScopeFail]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto movedFrom = sr::make_scope_fail(deleter);
    auto guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("move with copy init releases moved-from object", "[ScopeFail]")
{
    CallMock mock;
    const NotNothrowMoveMock notNothrow{&mock};
    REQUIRE_CALL(mock, deleter()).TIMES(0);
    sr::scope_fail<decltype(notNothrow)> movedFrom{notNothrow};
    auto guard = std::move(movedFrom);
}

TEST_CASE("move transfers state", "[ScopeFail]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto movedFrom = sr::make_scope_fail(deleter);
    auto guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("move transfers state if released", "[ScopeFail]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto movedFrom = sr::make_scope_fail(deleter);
    movedFrom.release();
    auto guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("deleter called on exception", "[ScopeFail]")
{
    try
    {
        REQUIRE_CALL(m, deleter());
        auto guard = sr::make_scope_fail(deleter);
        throw 3;
    }
    catch( ... )
    {
    }
}

TEST_CASE("deleter not called on pending exception", "[ScopeFail]")
{
    try
    {
        throw 3;
    }
    catch( ... )
    {
        auto guard = sr::make_scope_fail(deleter);
    }
}

