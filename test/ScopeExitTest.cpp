// MIT License
//
// Copyright (c) 2017-2026 offa
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "scope_exit.h"
#include "CallMocks.h"
#include <catch2/catch_test_macros.hpp>

namespace
{
    mock::CallMock m;

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
    mock::CallMock cm;
    REQUIRE_CALL(cm, deleter());
    [[maybe_unused]] auto guard = sr::scope_exit{[&cm]
                                                 { cm.deleter(); }};
}

TEST_CASE("exit function called and rethrow on copy exception", "[ScopeExit]")
{
    REQUIRE_THROWS([]
                   {
        const mock::ThrowOnCopyMock noMove;
        REQUIRE_CALL(noMove, deleter());

        sr::scope_exit guard{noMove}; }());
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
    mock::CallMock mock;
    const mock::NotNothrowMoveMock notNothrow{&mock};
    REQUIRE_CALL(mock, deleter());
    sr::scope_exit movedFrom{notNothrow};
    [[maybe_unused]] auto guard = std::move(movedFrom);
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
