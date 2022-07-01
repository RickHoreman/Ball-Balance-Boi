/**
 * @file       traits.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief Common type traits.
 */

#ifndef TT_TYPE_TRAITS_H
#define TT_TYPE_TRAITS_H

#include <type_traits>

/**
 * @namespace ts
 * @brief Type trait related components.
 */
namespace tt {

/**
 * @brief Checks if the given types are all distinct.
 * @tparam T Type to check.
 * @tparam Ts One or more types to check against.
 * @{
 */
template<typename T, typename... Ts>
struct is_distinct
    : std::conjunction<
        std::negation<std::is_same<T, Ts>>...,
        is_distinct<Ts...>> {};

template<typename T1, typename T2>
struct is_distinct<T1, T2>
    : std::negation<std::is_same<T1, T2>> {};

/**
 * @var is_distinct_v
 * @brief Helper variable template for the is_distinct type trait.
 */
template<typename T, typename... Ts>
inline constexpr auto is_distinct_v
    = bool{is_distinct<T, Ts...>{}};
/** @} */

} // namespace tt

#endif
