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

#include "scope_guard.h"
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

    CallMock m;


    void deleter()
    {
        m.deleter();
    }
}


TEST_CASE("deleter called on destruction", "[ScopeGuard]")
{
    REQUIRE_CALL(m, deleter());
    auto guard = sr::make_scope_exit(deleter);
    static_cast<void>(guard);
}

TEST_CASE("deleter lambda called on destruction", "[ScopeGuard]")
{
    CallMock cm;
    REQUIRE_CALL(cm, deleter());
    auto guard = sr::make_scope_exit([&cm] { cm.deleter(); });
    static_cast<void>(guard);
}

TEST_CASE("deleter called and rethrow on copy exception", "[ScopeGuard]")
{
    REQUIRE_THROWS([] {
        const ThrowOnCopyMock noMove;
        REQUIRE_CALL(noMove, deleter());

        sr::scope_guard_t<decltype(noMove)> guard{noMove};
        }());
}

TEST_CASE("deleter is not called if released", "[ScopeGuard]")
{
   REQUIRE_CALL(m, deleter()).TIMES(0);
   auto guard = sr::make_scope_exit(deleter);
   guard.release();
}

TEST_CASE("move releases moved-from object", "[ScopeGuard]")
{
    REQUIRE_CALL(m, deleter());
    auto movedFrom = sr::make_scope_exit(deleter);
    auto guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("move transfers state", "[ScopeGuard]")
{
    REQUIRE_CALL(m, deleter());
    auto movedFrom = sr::make_scope_exit(deleter);
    auto guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("move transfers state if released", "[ScopeGuard]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto movedFrom = sr::make_scope_exit(deleter);
    movedFrom.release();
    auto guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("no exception propagation from deleter", "[ScopeGuard]")
{
    REQUIRE_NOTHROW([] {
        auto guard = sr::make_scope_exit([] { throw std::exception{}; });
        static_cast<void>(guard);
        }());
}
