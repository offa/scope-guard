// MIT License
//
// Copyright (c) 2017-2025 offa
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

#include "scope_success.h"
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


TEST_CASE("exit function called on destruction", "[ScopeSuccess]")
{
    REQUIRE_CALL(m, deleter());
    [[maybe_unused]] auto guard = sr::scope_success{deleter};
}

TEST_CASE("exit function lambda called on destruction", "[ScopeSuccess]")
{
    mock::CallMock cm;
    REQUIRE_CALL(cm, deleter());
    [[maybe_unused]] auto guard = sr::scope_success{[&cm]
                                                    { cm.deleter(); }};
}

TEST_CASE("exit function not called and rethrow on copy exception", "[ScopeSuccess]")
{
    REQUIRE_THROWS([]
                   {
        const mock::ThrowOnCopyMock noMove;
        REQUIRE_CALL(noMove, deleter());
        sr::scope_success guard{noMove}; }());
}

TEST_CASE("exit function is not called if released", "[ScopeSuccess]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto guard = sr::scope_success{deleter};
    guard.release();
}

TEST_CASE("move releases moved-from object", "[ScopeSuccess]")
{
    REQUIRE_CALL(m, deleter());
    auto movedFrom = sr::scope_success{deleter};
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("move with copy init releases moved-from object", "[ScopeSuccess]")
{
    mock::CallMock mock;
    const mock::NotNothrowMoveMock notNothrow{&mock};
    REQUIRE_CALL(mock, deleter());
    sr::scope_success movedFrom{notNothrow};
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("move transfers state", "[ScopeSuccess]")
{
    REQUIRE_CALL(m, deleter());
    auto movedFrom = sr::scope_success{deleter};
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("move transfers state if released", "[ScopeSuccess]")
{
    REQUIRE_CALL(m, deleter()).TIMES(0);
    auto movedFrom = sr::scope_success{deleter};
    movedFrom.release();
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("exit function not called on exception", "[ScopeFail]")
{
    try
    {
        [[maybe_unused]] auto guard = sr::scope_success{deleter};
        throw std::exception{};
    }
    catch (...)
    {
    }
}

TEST_CASE("exit function called on pending exception", "[ScopeFail]")
{
    try
    {
        throw std::exception{};
    }
    catch (...)
    {
        REQUIRE_CALL(m, deleter());
        [[maybe_unused]] auto guard = sr::scope_success{deleter};
    }
}
