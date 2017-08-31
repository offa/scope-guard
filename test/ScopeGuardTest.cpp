
#include "scope_guard.h"
#include <catch.hpp>

using namespace guards;

TEST_CASE("deleter called on destruction", "[ScopeGuard]")
{
    bool executed = false;

    {
        auto guard = scope_guard([&executed] { executed = true; });
        static_cast<void>(guard);
    }

    REQUIRE(executed == true);
}

