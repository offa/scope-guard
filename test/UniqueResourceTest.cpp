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

using Handle = int;

TEST_CASE("deleter called on destruction", "[UniqueResource]")
{
    std::size_t calls{0};
    constexpr Handle handle{3};

    {
        auto guard = sr::unique_resource(handle, [&calls] { ++calls; });
        static_cast<void>(guard);
    }

    REQUIRE(calls == 1);
}

TEST_CASE("deleter is not called if released", "[UniqueResource]")
{
    std::size_t calls{0};
    constexpr Handle handle{3};

    {
        auto guard = sr::unique_resource(handle, [&calls] { ++calls; });
        guard.release();
    }

    REQUIRE(calls == 0);
}

TEST_CASE("release returns reference to resource", "[UniqueResource]")
{
    constexpr Handle handle{3};

    auto guard = sr::unique_resource(handle, [] { });
    const auto result = guard.release();

    REQUIRE(handle == result);
}

TEST_CASE("move releases moved-from object", "[UniqueResource]")
{
    std::size_t calls{0};
    constexpr Handle handle{3};

    {
        auto movedFrom = sr::unique_resource(handle, [&calls] { ++calls; });
        auto guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 1);
}

TEST_CASE("move transfers state", "[UniqueResource]")
{
    std::size_t calls{0};
    constexpr Handle handle{3};

    {
        auto movedFrom = sr::unique_resource(handle, [&calls] { ++calls; });
        auto guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 1);
}

TEST_CASE("move transfers state if released", "[UniqueResource]")
{
    std::size_t calls{0};
    constexpr Handle handle{3};

    {
        auto movedFrom = sr::unique_resource(handle, [&calls] { ++calls; });
        movedFrom.release();
        auto guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 0);
}

TEST_CASE("no exception propagation from deleter", "[UniqueResource]")
{
    constexpr Handle handle{3};

    REQUIRE_NOTHROW([] {
        auto guard = sr::unique_resource(handle, [] { throw "Don't propagate this!"; });
        static_cast<void>(guard);
        }());
}

