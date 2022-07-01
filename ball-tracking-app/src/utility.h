/**
 * @file       utility.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief Common utilities.
 */

#ifndef UTIL_UTILITY_H
#define UTIL_UTILITY_H

/**
 * @namespace util
 * @brief Utility related components.
 */
namespace util {

/**
 * @typedef access_ptr
 * @brief Non-owning pointer type.
 * @tparam T Type to point to.
 */
template<typename T>
using access_ptr = T*;

/**
 * @brief Variadic helper functor for std::visit.
 * @tparam Ts One or more callable types to inherit from.
 */
template<typename... Ts>
struct visitor : Ts... {
    using Ts::operator()...;
};

} // namespace util

#endif
