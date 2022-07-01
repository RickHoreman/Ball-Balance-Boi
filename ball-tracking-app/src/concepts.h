/**
 * @file       concepts.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief Common concepts.
 */

#ifndef CC_CONCEPTS_H
#define CC_CONCEPTS_H

#include "traits.h"

#include <concepts>
#include <string>
#include <string_view>

/**
 * @namespace cc
 * @brief Concept related components.
 */
namespace cc {

/**
 * @brief Constrains a type to be configurable.
 * @tparam T Type to check.
 */
template<typename T>
concept configurable = requires(T item, std::string_view value) {
    { item.name() } -> std::same_as<std::string const&>;
    { item.tagname() } -> std::same_as<std::string>;
    { item.template to<std::string>() } -> std::same_as<std::string>;
    { item.set(value) } -> std::same_as<void>;
};

/**
 * @brief Constrains a type to be an integral or floating point type.
 * @tparam T Type to check.
 */
template<typename T>
concept arithmetic = std::integral<T> or std::floating_point<T>;

/**
 * @brief Constrains all the given types to be distinct.
 * @tparam T Type to check.
 * @tparam Ts One or more types to check against.
 */
template<typename T, typename... Ts>
concept distinct = tt::is_distinct_v<T, Ts...>;

/**
 * @brief Constrains a type to be convertible to all the given types.
 * @tparam T Type to check.
 * @tparam Ts One or more types to check against.
 */
template<typename T, typename... Ts>
concept convertible_to_all = (std::convertible_to<T, Ts> and ...);

/**
 * @brief Constrains a type to be convertible to any of the given types.
 * @tparam T Type to check.
 * @tparam Ts One or more types to check against.
 */
template<typename T, typename... Ts>
concept convertible_to_any = (std::convertible_to<T, Ts> or ...);

/**
 * @brief Constrains a type to be convertible to none of the given types.
 * @tparam T Type to check.
 * @tparam Ts One or more types to check against.
 */
template<typename T, typename... Ts>
concept convertible_to_none = not convertible_to_any<T, Ts...>;

/**
 * @brief Constrains a type to be the same as all the given types.
 * @tparam T Type to check.
 * @tparam Ts One or more types to check against.
 */
template<typename T, typename... Ts>
concept same_as_all = (std::same_as<T, Ts> and ...);

/**
 * @brief Constrains a type to be the same as any of the given types.
 * @tparam T Type to check.
 * @tparam Ts One or more types to check against.
 */
template<typename T, typename... Ts>
concept same_as_any = (std::same_as<T, Ts> or ...);

/**
 * @brief Constrains a type to be the same as none of the given types.
 * @tparam T Type to check.
 * @tparam Ts One or more types to check against.
 */
template<typename T, typename... Ts>
concept same_as_none = not same_as_any<T, Ts...>;

} // namespace cc

#endif
