
#include "scope_guard.h"
#include <catch.hpp>

TEST_CASE("deleter called on destruction", "[ScopeGuard]")
{
    std::size_t calls{0};

    {
        auto guard = sr::scope_guard([&calls] { ++calls; });
        static_cast<void>(guard);
    }

    REQUIRE(calls == 1);
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
