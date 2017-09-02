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

struct CallMock
{
    MAKE_MOCK0(deleter, void());
};

namespace
{
    CallMock m;

    void deleter()
    {
        m.deleter();
    }
}


TEST_CASE("deleter called on destruction", "[ScopeGuard]")
{
    {
        REQUIRE_CALL(m, deleter());
        auto guard = sr::scope_guard(deleter);
        static_cast<void>(guard);
    }
}

TEST_CASE("deleter is not called if released", "[ScopeGuard]")
{
    std::size_t calls{0};

    {
        auto guard = sr::scope_guard([&calls] { ++calls; });
        guard.release();
    }

    REQUIRE(calls == 0);
}

TEST_CASE("move releases moved-from object", "[ScopeGuard]")
{
    std::size_t calls{0};

    {
        auto movedFrom = sr::scope_guard([&calls] { ++calls; });
        auto guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 1);
}

TEST_CASE("move transfers state", "[ScopeGuard]")
{
    std::size_t calls{0};

    {
        auto movedFrom = sr::scope_guard([&calls] { ++calls; });
        auto guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 1);
}

TEST_CASE("move transfers state if released", "[ScopeGuard]")
{
    std::size_t calls{0};

    {
        auto movedFrom = sr::scope_guard([&calls] { ++calls; });
        movedFrom.release();
        auto guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 0);
}

TEST_CASE("no exception propagation from deleter", "[ScopeGuard]")
{
    REQUIRE_NOTHROW([] {
        auto guard = sr::scope_guard([] { throw "Don't propagate this!"; });
        static_cast<void>(guard);
        }());
}
