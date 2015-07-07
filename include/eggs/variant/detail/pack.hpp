//! \file eggs/variant/detail/pack.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014-2015
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_DETAIL_PACK_HPP
#define EGGS_VARIANT_DETAIL_PACK_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

#include <eggs/variant/detail/config/prefix.hpp>

namespace eggs { namespace variants { namespace detail
{
    struct empty
    {
        EGGS_CXX11_CONSTEXPR bool operator==(empty) const { return true; }
        EGGS_CXX11_CONSTEXPR bool operator<(empty) const { return false; }
    };

    template <typename T>
    struct identity
    {
        using type = T;
    };

    template <std::size_t I>
    struct index
    {
        EGGS_CXX11_STATIC_CONSTEXPR std::size_t value = I;
    };

    template <typename ...Ts>
    struct pack
    {
        using type = pack;
        EGGS_CXX11_STATIC_CONSTEXPR std::size_t size = sizeof...(Ts);
    };

    template <typename T, T ...Vs>
    struct pack_c
    {
        using type = pack_c;
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename Left, typename Right>
    struct _make_index_pack_join;

    template <std::size_t... Left, std::size_t... Right>
    struct _make_index_pack_join<
        pack_c<std::size_t, Left...>
      , pack_c<std::size_t, Right...>
    > : pack_c<std::size_t, Left..., (sizeof...(Left) + Right)...>
    {};

    template <std::size_t N>
    struct _make_index_pack
      : _make_index_pack_join<
            typename _make_index_pack<N / 2>::type
          , typename _make_index_pack<N - N / 2>::type
        >
    {};

    template <>
    struct _make_index_pack<1>
      : pack_c<std::size_t, 0>
    {};

    template <>
    struct _make_index_pack<0>
      : pack_c<std::size_t>
    {};

    template <std::size_t N>
    using make_index_pack = typename _make_index_pack<N>::type;

    template <typename Ts>
    struct _index_pack;

    template <typename ...Ts>
    struct _index_pack<pack<Ts...>>
      : _make_index_pack<sizeof...(Ts)>
    {};

    template <typename Ts>
    using index_pack = typename _index_pack<Ts>::type;

    ///////////////////////////////////////////////////////////////////////////
    template <typename Vs>
    struct _make_typed_pack;

    template <std::size_t ...Vs>
    struct _make_typed_pack<pack_c<std::size_t, Vs...>>
      : pack<index<Vs>...>
    {};

    template <typename Ts>
    struct _typed_index_pack
      : _make_typed_pack<index_pack<Ts>>
    {};

    template <typename Ts>
    using typed_index_pack = typename _typed_index_pack<Ts>::type;

    ///////////////////////////////////////////////////////////////////////////
    template <typename Vs>
    struct all_of;

    template <bool ...Vs>
    struct all_of<pack_c<bool, Vs...>>
      : std::integral_constant<
            bool
          , std::is_same<
                pack_c<bool, Vs...>
              , pack_c<bool, (Vs || true)...> // true...
            >::value
        >
    {};

    template <typename ...Ts>
    struct all_of<pack<Ts...>>
      : all_of<pack_c<bool, (Ts::value)...>>
    {};

    template <typename ...Vs>
    struct any_of;

    template <bool ...Vs>
    struct any_of<pack_c<bool, Vs...>>
      : std::integral_constant<
            bool
          , !all_of<pack_c<bool, !Vs...>>::value
        >
    {};

    template <typename ...Ts>
    struct any_of<pack<Ts...>>
      : any_of<pack_c<bool, (Ts::value)...>>
    {};

    ///////////////////////////////////////////////////////////////////////////
    template <std::size_t I, typename T>
    struct _indexed {};

    template <typename Ts, typename Is = index_pack<Ts>>
    struct _indexer;

    template <typename ...Ts, std::size_t ...Is>
    struct _indexer<pack<Ts...>, pack_c<std::size_t, Is...>>
      : _indexed<Is, Ts>...
    {};

    empty _at_index(...);

    template <std::size_t I, typename T>
    identity<T> _at_index(_indexed<I, T> const&);

    template <std::size_t I, typename Ts>
    struct at_index
      : decltype(_at_index<I>(_indexer<Ts>{}))
    {};

    empty _index_of(...);

    template <typename T, std::size_t I>
    index<I> _index_of(_indexed<I, T> const&);

    template <typename T, typename Ts>
    struct index_of
      : decltype(_index_of<T>(_indexer<Ts>{}))
    {};

    ///////////////////////////////////////////////////////////////////////////
    template <typename Ts, typename Is>
    struct _split;

    template <typename ...Ts, std::size_t ...LowIs>
    struct _split<pack<Ts...>, pack_c<std::size_t, LowIs...>>
    {
        template <typename Is>
        struct _right;

        template <std::size_t ...HighIs>
        struct _right<pack_c<std::size_t, LowIs..., HighIs...>> {
            using type = pack<typename at_index<HighIs, pack<Ts...>>::type...>;
        };

        using left = pack<typename at_index<LowIs, pack<Ts...>>::type...>;
        using right = typename _right<index_pack<pack<Ts...>>>::type;
    };

    template <typename Ts>
    struct split;

    template <typename ...Ts>
    struct split<pack<Ts...>>
      : _split<pack<Ts...>, make_index_pack<sizeof...(Ts) / 2>>
    {};
}}}

#include <eggs/variant/detail/config/suffix.hpp>

#endif /*EGGS_VARIANT_DETAIL_PACK_HPP*/
