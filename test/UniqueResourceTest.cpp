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
#include <functional>
#include <catch.hpp>

using Handle = int;

TEST_CASE("deleter called on destruction", "[UniqueResource]")
{
    std::size_t calls{0};

    {
        auto guard = sr::unique_resource(Handle{3}, [&calls](auto) { ++calls; });
        static_cast<void>(guard);
    }

    REQUIRE(calls == 1);
}

TEST_CASE("deleter is not called if released", "[UniqueResource]")
{
    std::size_t calls{0};

    {
        auto guard = sr::unique_resource(Handle{3}, [&calls](auto) { ++calls; });
        guard.release();
    }

    REQUIRE(calls == 0);
}

TEST_CASE("release returns reference to resource", "[UniqueResource]")
{
    auto guard = sr::unique_resource(Handle{3}, [](auto) { });
    const auto result = guard.release();

    REQUIRE(3 == result);
}

TEST_CASE("move releases moved-from object", "[UniqueResource]")
{
    std::size_t calls{0};

    {
        auto movedFrom = sr::unique_resource(Handle{3}, [&calls](auto) { ++calls; });
        auto guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 1);
}

TEST_CASE("move transfers state", "[UniqueResource]")
{
    std::size_t calls{0};

    {
        auto movedFrom = sr::unique_resource(Handle{3}, [&calls](auto) { ++calls; });
        auto guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 1);
}

TEST_CASE("move transfers state if released", "[UniqueResource]")
{
    std::size_t calls{0};

    {
        auto movedFrom = sr::unique_resource(Handle{3}, [&calls](auto) { ++calls; });
        movedFrom.release();
        auto guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 0);
}

TEST_CASE("move assignment releases moved-from object", "[UniqueResource]")
{
    std::size_t calls{0};
    std::function<void(Handle)> del = [&calls](auto) { ++calls; };

    {
        auto movedFrom = sr::unique_resource(Handle{3}, del);
        auto guard = sr::unique_resource(Handle{3}, del);
        guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 2);
}

TEST_CASE("move assignment transfers state", "[UniqueResource]")
{
    std::size_t calls{0};
    std::function<void(Handle)> del = [&calls](auto) { ++calls; };

    {
        auto movedFrom = sr::unique_resource(Handle{3}, del);
        auto guard = sr::unique_resource(Handle{3}, del);
        guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 2);
}

TEST_CASE("move assignment transfers state if released", "[UniqueResource]")
{
    std::size_t calls{0};
    std::function<void(Handle)> del = [&calls](auto) { ++calls; };

    {
        auto movedFrom = sr::unique_resource(Handle{3}, del);
        movedFrom.release();
        auto guard = sr::unique_resource(Handle{3}, del);
        guard = std::move(movedFrom);
        static_cast<void>(guard);
    }

    REQUIRE(calls == 1);
}

TEST_CASE("no exception propagation from deleter", "[UniqueResource]")
{
    REQUIRE_NOTHROW([] {
        auto guard = sr::unique_resource(Handle{3}, [](auto) { throw "Don't propagate this!"; });
        static_cast<void>(guard);
        }());
}

TEST_CASE("invoke executes deleter on resource", "[UniqueResource]")
{
    std::size_t calls{0};

    {
        auto guard = sr::unique_resource(Handle{3}, [&calls](auto h)
                        {
                            REQUIRE(h == 3);
                            ++calls;
                        });
        guard.invoke();
    }

    REQUIRE(calls == 1);
}

TEST_CASE("invoke executes only multiple times if again strategy", "[UniqueResource]")
{
    std::size_t calls{0};

    {
        auto guard = sr::unique_resource(Handle{3}, [&calls](auto h) { if( h == 3 ) { ++calls;} });
        guard.invoke(sr::invoke_it::again);
        guard.invoke(sr::invoke_it::again);
    }

    REQUIRE(calls == 3);
}

TEST_CASE("invoke does nothing if released", "[UniqueResource]")
{
    std::size_t calls{0};

    {
        auto guard = sr::unique_resource(Handle{3}, [&calls](auto) { ++calls; });
        guard.release();
        guard.invoke(sr::invoke_it::once);
    }

    REQUIRE(calls == 0);
}

TEST_CASE("invoke executes after release if again strategy", "[UniqueResource]")
{
    std::size_t calls{0};

    {
        auto guard = sr::unique_resource(Handle{3}, [&calls](auto) { ++calls; });
        guard.release();
        guard.invoke(sr::invoke_it::again);
    }

    REQUIRE(calls == 1);
}

TEST_CASE("invoke does not propagate exception", "[UniqueResource]")
{
    auto guard = sr::unique_resource(Handle{3}, [](auto) { throw "Don't propagate this!"; });
    REQUIRE_NOTHROW(guard.invoke());
}

TEST_CASE("reset releases old ressource", "[UniqueResource]")
{
    std::size_t calls{0};

    {
        auto d = [&calls](auto v)
        {
            if( calls == 0 )
            {
                REQUIRE(v == 3);
            }
            else
            {
                REQUIRE(v == 7);
            }
            ++calls;
        };

        auto guard = sr::unique_resource(Handle{3}, d);
        guard.reset(Handle{7});
    }

    REQUIRE(calls == 2);
}

TEST_CASE("reset sets ressource", "[UniqueResource]")
{
    auto guard = sr::unique_resource(Handle{3}, [](auto) { });
    guard.reset(Handle{7});
    REQUIRE(guard.get() == 7);
}

TEST_CASE("get accesses ressource", "[UniqueResource]")
{
    auto guard = sr::unique_resource(Handle{3}, [](auto) { });
    REQUIRE(guard.get() == 3);
}

TEST_CASE("conversion operator accesses ressource", "[UniqueResource]")
{
    auto guard = sr::unique_resource(Handle{3}, [](auto) { });
    const auto& ref = guard;
    REQUIRE(ref == 3);
}

// TODO: conditional access functions

TEST_CASE("deleter access", "[UniqueResource]")
{
    std::size_t value{0};
    auto guard = sr::unique_resource(Handle{3}, [&value](auto v) { value = v; });
    REQUIRE(value == 0);
    guard.get_deleter()(6);
    REQUIRE(value == 6);

}
