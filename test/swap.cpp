// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2015
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <eggs/variant.hpp>
#include <string>

#include <eggs/variant/detail/config/prefix.hpp>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "constexpr.hpp"

EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = eggs::variant<>::npos;

TEST_CASE("variant<Ts...>::swap(variant<Ts...>&)", "[variant.swap]")
{
    SECTION("empty source")
    {
        eggs::variant<int, std::string> v1;

        REQUIRE(v1.which() == npos);

        eggs::variant<int, std::string> v2(42);

        REQUIRE(v2.which() == 0u);
        REQUIRE(*v2.target<int>() == 42);

        v2.swap(v1);

        CHECK(v1.which() == 0u);
        REQUIRE(v1.target<int>() != nullptr);
        CHECK(*v1.target<int>() == 42);
        CHECK(v2.which() == npos);

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1;
                eggs::variant<int, ConstexprTrivial> v2(ConstexprTrivial(42));
                v2.swap(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("empty target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2;

        REQUIRE(v2.which() == npos);

        v2.swap(v1);

        CHECK(v1.which() == npos);
        CHECK(v2.which() == 0u);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2;
                v2.swap(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("same target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(43);

        REQUIRE(v2.which() == v1.which());
        REQUIRE(*v2.target<int>() == 43);

        v2.swap(v1);

        CHECK(v1.which() == v1.which());
        REQUIRE(v1.target<int>() != nullptr);
        CHECK(*v1.target<int>() == 43);
        CHECK(v2.which() == 0u);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2(ConstexprTrivial(43));
                v2.swap(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }

    SECTION("different target")
    {
        eggs::variant<int, std::string> v1(42);

        REQUIRE(v1.which() == 0u);
        REQUIRE(*v1.target<int>() == 42);

        eggs::variant<int, std::string> v2(std::string{""});

        REQUIRE(v2.which() == 1u);
        REQUIRE(*v2.target<std::string>() == "");

        v2.swap(v1);

        CHECK(v1.which() == 1u);
        REQUIRE(v1.target<std::string>() != nullptr);
        CHECK(*v1.target<std::string>() == "");
        CHECK(v2.which() == 0u);
        REQUIRE(v2.target<int>() != nullptr);
        CHECK(*v2.target<int>() == 42);

#if EGGS_CXX14_HAS_CONSTEXPR
        SECTION("constexpr")
        {
            struct test { static constexpr int call()
            {
                eggs::variant<int, ConstexprTrivial> v1(ConstexprTrivial(42));
                eggs::variant<int, ConstexprTrivial> v2(43);
                v2.swap(v1);
                return 0;
            }};
            constexpr int c = test::call();
        }
#endif
    }
}

TEST_CASE("variant<>::swap(variant<>&)", "[variant.swap]")
{
    eggs::variant<> v1;

    REQUIRE(v1.which() == npos);

    eggs::variant<> v2;

    REQUIRE(v2.which() == npos);

    v2.swap(v1);

    CHECK(v1.which() == npos);
    CHECK(v2.which() == npos);

#if EGGS_CXX14_HAS_CONSTEXPR
    SECTION("constexpr")
    {
        struct test { static constexpr int call()
        {
            eggs::variant<> v1;
            eggs::variant<> v2;
            v2.swap(v1);
            return 0;
        }};
        constexpr int c = test::call();
    }
#endif
}
