// MIT License
//
// Copyright (c) 2017-2022 offa
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

#include "scope_fail.h"
#include "CallMocks.h"
#include <catch2/catch.hpp>

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


TEST_CASE("exit function called on destruction", "[ScopeFail]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    [[maybe_unused]] auto guard = sr::scope_fail{deleter};
}

TEST_CASE("exit function lambda called on destruction", "[ScopeFail]")
{
    CallMock cm;
    REQUIRE_CALL(cm, deleter()).TIMES(0);
    [[maybe_unused]] auto guard = sr::scope_fail{[&cm] { cm.deleter(); }};
}

TEST_CASE("exit function called and rethrow on copy exception", "[ScopeFail]")
{
    REQUIRE_THROWS([] {
        const ThrowOnCopyMock noMove;
        REQUIRE_CALL(noMove, deleter());

        sr::scope_fail guard{noMove};
    }());
}

TEST_CASE("exit function is not called if released", "[ScopeFail]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto guard = sr::scope_fail{deleter};
    guard.release();
}

TEST_CASE("move releases moved-from object", "[ScopeFail]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto movedFrom = sr::scope_fail{deleter};
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("move with copy init releases moved-from object", "[ScopeFail]")
{
    CallMock mock;
    const NotNothrowMoveMock notNothrow{&mock};
    REQUIRE_CALL(mock, deleter()).TIMES(0);
    sr::scope_fail movedFrom{notNothrow};
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("move transfers state", "[ScopeFail]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto movedFrom = sr::scope_fail{deleter};
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("move transfers state if released", "[ScopeFail]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto movedFrom = sr::scope_fail{deleter};
    movedFrom.release();
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("exit function called on exception", "[ScopeFail]")
{
    try
    {
        REQUIRE_CALL(m, deleter());
        [[maybe_unused]] auto guard = sr::scope_fail{deleter};
        throw std::exception{};
    }
    catch (...)
    {
    }
}

TEST_CASE("exit function not called on pending exception", "[ScopeFail]")
{
    try
    {
        throw std::exception{};
    }
    catch (...)
    {
        [[maybe_unused]] auto guard = sr::scope_fail{deleter};
    }
}
