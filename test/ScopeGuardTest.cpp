
#include "scope_guard.h"
#include <catch.hpp>

TEST_CASE("deleter called on destruction", "[ScopeGuard]")
{
    bool executed = false;

    {
        auto guard = sg::scope_guard([&executed] { executed = true; });
        static_cast<void>(guard);
    }

    REQUIRE(executed == true);
}

TEST_CASE("deleter is not called if released", "[ScopeGuard]")
{
    bool executed = false;

    {
        auto guard = sg::scope_guard([&executed] { executed = true; });
        guard.release();
    }

    REQUIRE(executed == false);
}

