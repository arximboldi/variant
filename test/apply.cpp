// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2015
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <sstream>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

struct fun
{
    std::size_t nonconst_lvalue, const_lvalue, rvalue;

    fun() : nonconst_lvalue{0}, const_lvalue{0}, rvalue{0} {}

    template <typename T>
    static std::string to_string(T const& t)
    {
        std::ostringstream oss;
        oss << t;
        return oss.str();
    }

    template <typename T, typename U>
    static std::string to_string(T const& t, U const& u)
    {
        std::ostringstream oss;
        oss << t << ',' << u;
        return oss.str();
    }

    template <typename T>
    std::string operator()(T& t)
    {
        ++nonconst_lvalue;
        return to_string(t);
    }

    template <typename T, typename U>
    std::string operator()(T& t, U& u)
    {
        ++nonconst_lvalue;
        return to_string(t, u);
    }

    template <typename T>
    std::string operator()(T const& t)
    {
        ++const_lvalue;
        return to_string(t);
    }

    template <typename T, typename U>
    std::string operator()(T const& t, U const& u)
    {
        ++const_lvalue;
        return to_string(t, u);
    }

    template <typename T>
    std::string operator()(T&& t)
    {
        ++rvalue;
        return to_string(t);
    }

    template <typename T, typename U>
    std::string operator()(T&& t, U&& u)
    {
        ++rvalue;
        return to_string(t, u);
    }
};

#if EGGS_CXX11_HAS_CONSTEXPR
struct constexpr_fun
{
    template <typename T>
    constexpr std::size_t operator()(T const&) const
    {
        return sizeof(T);
    }

    template <typename T, typename U>
    constexpr std::size_t operator()(T const&, U const&) const
    {
        return sizeof(T) + sizeof(U);
    }
};
#endif

TEST_CASE("apply<R>(F&&, variant<Ts...>&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::string ret = eggs::variants::apply<std::string>(f, v);

    CHECK(f.nonconst_lvalue == 1u);
    CHECK(ret == "42");

#if EGGS_CXX98_HAS_EXCEPTIONS
    SECTION("throws")
    {
        eggs::variant<int, std::string> empty;

        REQUIRE(empty.which() == npos);

        CHECK_THROWS_AS(
            eggs::variants::apply<void>(fun{}, empty)
          , eggs::variants::bad_variant_access);
    }
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(Constexpr(42));
            std::size_t ar = eggs::variants::apply<std::size_t>(constexpr_fun{}, v);
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}

TEST_CASE("apply<R>(F&&, variant<Ts...> const&)", "[variant.apply]")
{
    eggs::variant<int, std::string> const v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::string ret = eggs::variants::apply<std::string>(f, v);

    CHECK(f.const_lvalue == 1u);
    CHECK(ret == "42");

#if EGGS_CXX98_HAS_EXCEPTIONS
    SECTION("throws")
    {
        eggs::variant<int, std::string> const empty;

        REQUIRE(empty.which() == npos);

        CHECK_THROWS_AS(
            eggs::variants::apply<void>(fun{}, empty)
          , eggs::variants::bad_variant_access);
    }
#endif

#if EGGS_CXX11_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        constexpr eggs::variant<int, Constexpr> v(Constexpr(42));
        constexpr std::size_t ar = eggs::variants::apply<std::size_t>(constexpr_fun{}, v);
    }
#endif
}

TEST_CASE("apply<R>(F&&, variant<Ts...>&&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::string ret = eggs::variants::apply<std::string>(f, std::move(v));

    CHECK(f.rvalue == 1u);
    CHECK(ret == "42");

#if EGGS_CXX98_HAS_EXCEPTIONS
    SECTION("throws")
    {
        eggs::variant<int, std::string> empty;

        REQUIRE(empty.which() == npos);

        CHECK_THROWS_AS(
            eggs::variants::apply<void>(fun{}, std::move(empty))
          , eggs::variants::bad_variant_access);
    }
#endif

#if EGGS_CXX14_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(Constexpr(42));
            std::size_t ar = eggs::variants::apply<std::size_t>(constexpr_fun{}, std::move(v));
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}

#if EGGS_CXX98_HAS_EXCEPTIONS
TEST_CASE("apply<R>(F&&, variant<>&)", "[variant.apply]")
{
    eggs::variant<> v;

    REQUIRE(v.which() == npos);

    CHECK_THROWS_AS(
        eggs::variants::apply<void>(fun{}, v)
      , eggs::variants::bad_variant_access);
}

TEST_CASE("apply<R>(F&&, variant<> const&)", "[variant.apply]")
{
    eggs::variant<> const v;

    REQUIRE(v.which() == npos);

    CHECK_THROWS_AS(
        eggs::variants::apply<void>(fun{}, std::move(v))
      , eggs::variants::bad_variant_access);
}

TEST_CASE("apply<R>(F&&, variant<>&&)", "[variant.apply]")
{
    eggs::variant<> v;

    REQUIRE(v.which() == npos);

    CHECK_THROWS_AS(
        eggs::variants::apply<void>(fun{}, std::move(v))
      , eggs::variants::bad_variant_access);
}
#endif

TEST_CASE("apply(F&&, variant<Ts...>&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::string ret = eggs::variants::apply(f, v);

    CHECK(f.nonconst_lvalue == 1u);
    CHECK(ret == "42");

#if EGGS_CXX14_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(Constexpr(42));
            std::size_t ar = eggs::variants::apply<std::size_t>(constexpr_fun{}, v);
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}

TEST_CASE("apply(F&&, variant<Ts...> const&)", "[variant.apply]")
{
    eggs::variant<int, std::string> const v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::string ret = eggs::variants::apply(f, v);

    CHECK(f.const_lvalue == 1u);
    CHECK(ret == "42");

#if EGGS_CXX11_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        constexpr eggs::variant<int, Constexpr> v(Constexpr(42));
        constexpr std::size_t ar = eggs::variants::apply<std::size_t>(constexpr_fun{}, v);
    }
#endif
}

TEST_CASE("apply(F&&, variant<Ts...>&&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v(42);

    REQUIRE(v.which() == 0u);
    REQUIRE(*v.target<int>() == 42);

    fun f;
    std::string ret = eggs::variants::apply(f, std::move(v));

    CHECK(f.rvalue == 1u);
    CHECK(ret == "42");

#if EGGS_CXX14_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v(Constexpr(42));
            std::size_t ar = eggs::variants::apply<std::size_t>(constexpr_fun{}, std::move(v));
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}

TEST_CASE("apply<R>(F&&, variant<Ts...>&, variant<Us...>&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v1(42);

    REQUIRE(v1.which() == 0u);
    REQUIRE(*v1.target<int>() == 42);

    eggs::variant<std::string, int> v2(std::string{"43"});

    REQUIRE(v2.which() == 0u);
    REQUIRE(*v2.target<std::string>() == "43");

    fun f;
    std::string ret = eggs::variants::apply<std::string>(f, v1, v2);

    CHECK(f.nonconst_lvalue == 1u);
    CHECK(ret == "42,43");

#if EGGS_CXX14_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v1(Constexpr(42));
            eggs::variant<int, Constexpr> v2(43);
            std::size_t ar = eggs::variants::apply<std::size_t>(constexpr_fun{}, v1, v2);
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}

TEST_CASE("apply<R>(F&&, variant<Ts...> const&, variant<Us...> const&)", "[variant.apply]")
{
    eggs::variant<int, std::string> const v1(42);

    REQUIRE(v1.which() == 0u);
    REQUIRE(*v1.target<int>() == 42);

    eggs::variant<std::string, int> const v2(std::string{"43"});

    REQUIRE(v2.which() == 0u);
    REQUIRE(*v2.target<std::string>() == "43");

    fun f;
    std::string ret = eggs::variants::apply<std::string>(f, v1, v2);

    CHECK(f.const_lvalue == 1u);
    CHECK(ret == "42,43");

#if EGGS_CXX11_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        constexpr eggs::variant<int, Constexpr> v1(Constexpr(42));
        constexpr eggs::variant<int, Constexpr> v2(43);
        constexpr std::size_t ar = eggs::variants::apply<std::size_t>(constexpr_fun{}, v1, v2);
    }
#endif
}

TEST_CASE("apply<R>(F&&, variant<Ts...>&&, variant<Us...>&&)", "[variant.apply]")
{
    eggs::variant<int, std::string> v1(42);

    REQUIRE(v1.which() == 0u);
    REQUIRE(*v1.target<int>() == 42);

    eggs::variant<std::string, int> v2(std::string{"43"});

    REQUIRE(v2.which() == 0u);
    REQUIRE(*v2.target<std::string>() == "43");

    fun f;
    std::string ret = eggs::variants::apply<std::string>(f, std::move(v1), std::move(v2));

    CHECK(f.rvalue == 1u);
    CHECK(ret == "42,43");

#if EGGS_CXX14_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        struct test { static constexpr int call()
        {
            eggs::variant<int, Constexpr> v1(Constexpr(42));
            eggs::variant<int, Constexpr> v2(43);
            std::size_t ar = eggs::variants::apply<std::size_t>(constexpr_fun{}, std::move(v1), std::move(v2));
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}
