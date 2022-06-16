/**
 * @file       concepts.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief ..
 * @details ..
 */

#ifndef CC_CONCEPTS_H
#define CC_CONCEPTS_H

#include "traits.h"

#include <concepts>

/**
 * @namespace ..
 * @brief ..
 */
namespace cc {

/**
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 */
template<typename T, typename... Ts>
concept same_as_all = ts::is_same_all_v<T, Ts...>;

/**
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 */
template<typename T, typename... Ts>
concept same_as_any = ts::is_same_any_v<T, Ts...>;

/**
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 */
template<typename T, typename... Ts>
concept same_as_none = ts::is_same_none_v<T, Ts...>;

/**
 * @brief ..
 * @details ..
 * @tparam ..
 */
template<typename T>
concept arithmetic = std::integral<T> or std::floating_point<T>;

} // namespace con

#endif
