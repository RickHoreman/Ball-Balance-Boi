/**
 * @file       traits.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief ..
 * @details ..
 */

#ifndef TS_TYPE_TRAITS_H
#define TS_TYPE_TRAITS_H

#include <type_traits>

/**
 * @namespace ..
 * @brief ..
 */
namespace ts {

/**
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 * @{
 */
template<typename T, typename... Ts>
struct is_same_all
    : std::conjunction<std::is_same<T, Ts>...> {};

/**
 * @brief ..
 */
template<typename T, typename... Ts>
inline constexpr auto is_same_all_v
    = bool{is_same_all<T, Ts...>{}};
/** @} */

/**
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 * @{
 */
template<typename T, typename... Ts>
struct is_same_any
    : std::disjunction<
        std::is_same<T, Ts>...,
        is_same_any<Ts...>> {};

template<typename T1, typename T2>
struct is_same_any<T1, T2>
    : std::is_same<T1, T2> {};

/**
 * @brief ..
 */
template<typename T, typename... Ts>
inline constexpr auto is_same_any_v
    = bool{is_same_any<T, Ts...>{}};
/** @} */

/**
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 * @{
 */
template<typename T, typename... Ts>
struct is_same_none
    : std::negation<is_same_any<T, Ts...>> {};

/**
 * @brief ..
 */
template<typename T, typename... Ts>
inline constexpr auto is_same_none_v
    = bool{is_same_none<T, Ts...>{}};
/** @} */

} // namespace tts

#endif
