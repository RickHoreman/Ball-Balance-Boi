/**
 * @file       types.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief Common types.
 */

#ifndef TYPE_TYPES_H
#define TYPE_TYPES_H

#include <cstdint>

/**
 * @namespace type
 * @brief Type related components.
 */
inline namespace type {

/**
 * @brief Shorthand for the fixed width integral types.
 * @{
 */
using int8   = std::int8_t;
using uint8  = std::uint8_t;
using int16  = std::int16_t;
using uint16 = std::uint16_t;
using int32  = std::int32_t;
using uint32 = std::uint32_t;
using int64  = std::int64_t;
using uint64 = std::uint64_t;
/** @} */

/**
 * @brief Literals for the fixed width integral types.
 * @{
 */
consteval auto operator""  _i8(unsigned long long int n) noexcept -> int8
{ return {static_cast<int8>(n)}; }

consteval auto operator""  _u8(unsigned long long int n) noexcept -> uint8
{ return {static_cast<uint8>(n)}; }

consteval auto operator"" _i16(unsigned long long int n) noexcept -> int16
{ return {static_cast<int16>(n)}; }

consteval auto operator"" _u16(unsigned long long int n) noexcept -> uint16
{ return {static_cast<uint16>(n)}; }

consteval auto operator"" _i32(unsigned long long int n) noexcept -> int32
{ return {static_cast<int32>(n)}; }

consteval auto operator"" _u32(unsigned long long int n) noexcept -> uint32
{ return {static_cast<uint32>(n)}; }

consteval auto operator"" _i64(unsigned long long int n) noexcept -> int64
{ return {static_cast<int64>(n)}; }

consteval auto operator"" _u64(unsigned long long int n) noexcept -> uint64
{ return {static_cast<uint64>(n)}; }
/** @} */

} // namespace type

#endif
