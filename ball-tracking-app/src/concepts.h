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
 */
template<typename T>
concept arithmetic = std::integral<T> or std::floating_point<T>;

} // namespace cc

#endif
