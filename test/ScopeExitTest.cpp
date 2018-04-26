/*
 * Scope Guard
 * Copyright (C) 2017-2018  offa
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

#include "scope_exit.h"
#include "CallMocks.h"
#include <catch.hpp>

using namespace mock;
using namespace trompeloeil;

namespace
{
    CallMock m;

    void deleter()
    {
        m.deleter();
    }
}


TEST_CASE("exit function called on destruction", "[ScopeExit]")
{
    REQUIRE_CALL(m, deleter());
    [[maybe_unused]] auto guard = sr::scope_exit{deleter};
}

TEST_CASE("exit function lambda called on destruction", "[ScopeExit]")
{
    CallMock cm;
    REQUIRE_CALL(cm, deleter());
    [[maybe_unused]] auto guard = sr::scope_exit{[&cm] { cm.deleter(); }};
}

TEST_CASE("exit function called and rethrow on copy exception", "[ScopeExit]")
{
    REQUIRE_THROWS([] {
        const ThrowOnCopyMock noMove;
        REQUIRE_CALL(noMove, deleter());

        sr::scope_exit guard{noMove};
    }());
}

TEST_CASE("exit function is not called if released", "[ScopeExit]")
{
   REQUIRE_CALL(m, deleter()).TIMES(0);
   auto guard = sr::scope_exit{deleter};
   guard.release();
}

TEST_CASE("move releases moved-from object", "[ScopeExit]")
{
    REQUIRE_CALL(m, deleter());
    auto movedFrom = sr::scope_exit{deleter};
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("move with copy init releases moved-from object", "[ScopeExit]")
{
    CallMock mock;
    const NotNothrowMoveMock notNothrow{&mock};
    REQUIRE_CALL(mock, deleter());
    sr::scope_exit movedFrom{notNothrow};
    auto guard = std::move(movedFrom);
}

TEST_CASE("move transfers state", "[ScopeExit]")
{
    REQUIRE_CALL(m, deleter());
    auto movedFrom = sr::scope_exit{deleter};
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("move transfers state if released", "[ScopeExit]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto movedFrom = sr::scope_exit{deleter};
    movedFrom.release();
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

