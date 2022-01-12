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

#include "unique_resource.h"
#include "CallMocks.h"
#include <catch2/catch.hpp>
#include <trompeloeil.hpp>

namespace
{
    mock::CallMock m;

    void deleter(mock::Handle h)
    {
        m.deleter(h);
    }
}

TEST_CASE("default construction", "[UniqueResource]")
{
    [[maybe_unused]] sr::unique_resource<int, mock::MoveableMock> guard{};
}

TEST_CASE("construction with move", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    [[maybe_unused]] auto guard = sr::unique_resource{mock::Handle{3}, deleter};
}

TEST_CASE("construction with copy", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    const mock::Handle h{3};
    const auto d = [](auto v) { m.deleter(v); };
    [[maybe_unused]] auto guard = sr::unique_resource{h, d};
}

TEST_CASE("construction with copy calls deleter and rethrows on failed copy", "[UniqueResource]")
{
    REQUIRE_THROWS([] {
        const mock::ThrowOnCopyMock noMove;
        const auto d = [](const auto&) { m.deleter(3); };
        REQUIRE_CALL(m, deleter(3));

        [[maybe_unused]] sr::unique_resource guard{noMove, d};
    }());
}

TEST_CASE("move-construction with move", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    auto movedFrom = sr::unique_resource{mock::Handle{3}, deleter};
    auto guard = std::move(movedFrom);
    CHECK(guard.get() == 3);
}

TEST_CASE("move-construction with copy", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(7));
    auto d = [](auto) { deleter(7); };

    const mock::CopyMock copyMock;
    sr::unique_resource movedFrom{copyMock, d};
    [[maybe_unused]] auto guard = std::move(movedFrom);
}

TEST_CASE("move-construction prevents double release", "[UniqueResource]")
{
    auto movedFrom = sr::unique_resource{mock::Handle{3}, mock::ConditionalThrowOnCopyDeleter{}};
    movedFrom.release();
    mock::ConditionalThrowOnCopyDeleter::throwOnNextCopy = true;

    REQUIRE_THROWS([&movedFrom] { [[maybe_unused]] auto guard = std::move(movedFrom); }());
}

TEST_CASE("move assignment calls deleter", "[UniqueResource]")
{
    auto moveFrom = sr::unique_resource{mock::Handle{3}, deleter};
    REQUIRE_CALL(m, deleter(4));

    {
        REQUIRE_CALL(m, deleter(3));
        auto guard = sr::unique_resource{mock::Handle{4}, deleter};
        guard = std::move(moveFrom);
    }
}

TEST_CASE("deleter called on destruction", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    [[maybe_unused]] auto guard = sr::unique_resource{mock::Handle{3}, deleter};
}

TEST_CASE("reset calls deleter", "[UniqueResource]")
{
    auto guard = sr::unique_resource{mock::Handle{3}, deleter};

    {
        REQUIRE_CALL(m, deleter(3));
        guard.reset();
    }
}

TEST_CASE("reset does not call deleter if released", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3)).TIMES(0);
    auto guard = sr::unique_resource{mock::Handle{3}, deleter};
    guard.release();
    guard.reset();
}

TEST_CASE("reset sets new rvalue and calls deleter on previous", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    REQUIRE_CALL(m, deleter(7));
    auto guard = sr::unique_resource{mock::Handle{3}, deleter};
    guard.reset(mock::Handle{7});
}

TEST_CASE("reset sets new lvalue and calls deleter on previous", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    REQUIRE_CALL(m, deleter(7));
    auto d = [](const auto& v) { deleter(v.value); };
    auto guard = sr::unique_resource{mock::NotNothrowAssignable{3}, d};
    const mock::NotNothrowAssignable h{7};
    guard.reset(h);
}

TEST_CASE("reset handles exception on assignment", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    REQUIRE_CALL(m, deleter(7));
    auto d = [](const auto& v) { deleter(v.handle); };
    auto guard = sr::unique_resource{mock::ConditionalThrowOnCopyMock{3, false}, d};
    guard.reset(mock::ConditionalThrowOnCopyMock{7, true});
}

TEST_CASE("release disables deleter", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3)).TIMES(0);
    auto guard = sr::unique_resource{mock::Handle{3}, deleter};
    guard.release();
}

TEST_CASE("get returns resource", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    auto guard = sr::unique_resource{mock::Handle{3}, deleter};
    CHECK(guard.get() == 3);
}

TEST_CASE("pointer access returns resource", "[UniqueResource]")
{
    const auto p = std::make_pair(3, 4);
    auto guard = sr::unique_resource{&p, [](auto*) {}};
    REQUIRE(guard->first == 3);
    REQUIRE(guard->second == 4);
}

TEST_CASE("pointer dereference returns resource", "[UniqueResource]")
{
    mock::Handle h{5};
    auto guard = sr::unique_resource{mock::PtrHandle{&h}, [](auto*) {}};
    REQUIRE(*guard == 5);
}

TEST_CASE("deleter access", "[UniqueResource]")
{
    auto guard = sr::unique_resource{mock::Handle{3}, deleter};
    guard.release();

    {
        REQUIRE_CALL(m, deleter(8));
        guard.get_deleter()(8);
    }
}

TEST_CASE("make unique resource", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(7));
    [[maybe_unused]] auto guard = sr::unique_resource{mock::Handle{7}, deleter};
}

TEST_CASE("make unique resource with reference wrapper", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(3));
    mock::Handle h{3};
    [[maybe_unused]] auto guard = sr::unique_resource{std::ref(h), deleter};
}

TEST_CASE("make unique resource checked", "[UniqueResource]")
{
    REQUIRE_CALL(m, deleter(4));
    [[maybe_unused]] auto guard = sr::make_unique_resource_checked(mock::Handle{4}, mock::Handle{-1}, deleter);
}

TEST_CASE("make unique resource checked releases if invalid", "[UniqueResource]")
{
    [[maybe_unused]] auto guard = sr::make_unique_resource_checked(mock::Handle{-1}, mock::Handle{-1}, deleter);
}

TEST_CASE("not noexcept move", "[UniqueResource]")
{
    mock::NoexceptDeleter<false> deleter{};
    auto guard = sr::unique_resource{mock::NoexceptResource<false>{}, deleter};
    auto temp = std::move(guard);
    guard = std::move(temp);
}

TEST_CASE("noexcept move", "[UniqueResource]")
{
    mock::NoexceptDeleter<true> deleter{};
    auto guard = sr::unique_resource{mock::NoexceptResource<true>{}, deleter};
    auto temp = std::move(guard);
    guard = std::move(temp);
}

TEST_CASE("std::function deleter", "[UniqueResource]")
{
    const auto deleter = std::function<void(mock::Handle)>{[]([[maybe_unused]] mock::Handle h) {}};
    sr::unique_resource movedFrom{mock::Handle{3}, deleter};
    sr::unique_resource guard2{mock::Handle{4}, deleter};
    guard2 = std::move(movedFrom);
}

TEST_CASE("not noexcept move and copy assignable deleter", "[UniqueResource]")
{
    sr::unique_resource movedFrom{0, mock::FunctionDeleter{}};
    [[maybe_unused]] sr::unique_resource guard{0, mock::FunctionDeleter{}};
    guard = std::move(movedFrom);
}
