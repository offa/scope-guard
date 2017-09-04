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
#include <trompeloeil.hpp>

using namespace trompeloeil;

using Handle = int;
using PtrHandle = std::add_pointer_t<Handle>;

namespace mock
{
    struct CallMock
    {
        MAKE_MOCK1(deleter, void(Handle));
    };
}


namespace
{
    mock::CallMock m;

    void deleter(Handle h)
    {
        m.deleter(h);
    }
}


TEST_CASE("deleter called on destruction", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));

    auto guard = sr::unique_resource(Handle{3}, deleter);
    static_cast<void>(guard);
}

TEST_CASE("deleter is not called if released", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3)).TIMES(0);
    auto guard = sr::unique_resource(Handle{3}, deleter);
    guard.release();
}

TEST_CASE("deleter called if checked valid", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    auto guard = sr::unique_resource_checked(Handle{3}, Handle{6}, deleter);
    static_cast<void>(guard);
}

TEST_CASE("deleter not called if checked invalid", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3)).TIMES(0);
    auto guard = sr::unique_resource_checked(Handle{3}, Handle{3}, deleter);
    static_cast<void>(guard);
}

TEST_CASE("release returns reference to resource", "[UniqueResource]")
{
    auto guard = sr::unique_resource(Handle{3}, [](auto) { });
    const auto result = guard.release();

    REQUIRE(3 == result);
}

TEST_CASE("move releases moved-from object", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    auto movedFrom = sr::unique_resource(Handle{3}, deleter);
    auto guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("move transfers state", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    auto movedFrom = sr::unique_resource(Handle{3}, deleter);
    auto guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("move transfers state if released", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(ANY(Handle))).TIMES(0); // TODO: Use ANY(T)

    auto movedFrom = sr::unique_resource(Handle{3}, deleter);
    movedFrom.release();
    auto guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("move assignment releases moved-from object", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(5));
    REQUIRE_CALL(m, deleter(3));

    auto movedFrom = sr::unique_resource(Handle{3}, deleter);
    auto guard = sr::unique_resource(Handle{5}, deleter);
    guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("move assignment transfers state", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(5));
    REQUIRE_CALL(m, deleter(3));

    auto movedFrom = sr::unique_resource(Handle{3}, deleter);
    auto guard = sr::unique_resource(Handle{5}, deleter);
    guard = std::move(movedFrom);
    static_cast<void>(guard);
}

TEST_CASE("move assignment transfers state if released", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(5));

    auto movedFrom = sr::unique_resource(Handle{3}, deleter);
    movedFrom.release();
    auto guard = sr::unique_resource(Handle{5}, deleter);
    guard = std::move(movedFrom);
    static_cast<void>(guard);
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
    REQUIRE_CALL(m, deleter(3));

    auto guard = sr::unique_resource(Handle{3}, deleter);
    guard.invoke();
}

TEST_CASE("invoke executes only multiple times if again strategy", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3)).TIMES(3);

    auto guard = sr::unique_resource(Handle{3}, deleter);
    guard.invoke(sr::invoke_it::again);
    guard.invoke(sr::invoke_it::again);
}

TEST_CASE("invoke does nothing if released", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(ANY(Handle))).TIMES(0);

    auto guard = sr::unique_resource(Handle{3}, deleter);
    guard.release();
    guard.invoke(sr::invoke_it::once);
}

TEST_CASE("invoke executes after release if again strategy", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));

    auto guard = sr::unique_resource(Handle{3}, deleter);
    guard.release();
    guard.invoke(sr::invoke_it::again);
}

TEST_CASE("invoke does not propagate exception", "[UniqueResource]")
{
    auto guard = sr::unique_resource(Handle{3}, [](auto) { throw "Don't propagate this!"; });
    REQUIRE_NOTHROW(guard.invoke());
}

TEST_CASE("reset releases old ressource", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    REQUIRE_CALL(m, deleter(7));

    auto guard = sr::unique_resource(Handle{3}, deleter);
    guard.reset(Handle{7});
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

TEST_CASE("conversion operator", "[UniqueResource]")
{
    auto guard = sr::unique_resource(Handle{3}, [](auto) { });
    const auto& ref = guard;
    REQUIRE(ref == 3);
}

TEST_CASE("pointer access operator" "[UniqueResource]")
{
    const auto p = std::make_pair(3, 4);
    auto guard = sr::unique_resource(&p, [](auto) { });
    const auto x = guard.operator->();
    REQUIRE(x->first == 3);
}

TEST_CASE("dereference operator", "[UniqueResource]")
{
    Handle h{4};
    auto guard = sr::unique_resource(PtrHandle{&h}, [](auto) { });
    const auto x = guard.operator*();
    REQUIRE(x == 4);
}

TEST_CASE("deleter access", "[UniqueResource]")
{
    std::size_t value{0};
    auto guard = sr::unique_resource(Handle{3}, [&value](auto v) { value = v; });
    REQUIRE(value == 0);
    guard.get_deleter()(6);
    REQUIRE(value == 6);
}

