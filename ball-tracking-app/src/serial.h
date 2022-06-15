/**
 * @file       serial.h
 * @version    0.1
 * @date       .. ..
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief ..
 * @details ..
 */

#ifndef COMM_SERIAL_H
#define COMM_SERIAL_H

#include <stdexcept>
#include <string>
#include <string_view>

/**
 * @namespace ..
 * @brief ..
 */
namespace comm {

/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct serial_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * @interface ..
 * @brief ..
 * @details ..
 */
class serial {
public:
    /**
     * @brief ..
     * @details ..
     * @{
     */
    constexpr serial() = default;
    constexpr virtual ~serial() = default;
    /** @} */

    /**
     * @brief ..
     * @details ..
     */
    constexpr auto setup() -> void
    { setup_impl(); }

    /**
     * @brief ..
     * @details ..
     * @param[in] .. ..
     * @return ..
     */
    constexpr auto read(long n_bytes = 256) const -> std::string
    { return read_impl(n_bytes); }

    /**
     * @brief ..
     * @details ..
     * @param[in] .. ..
     */
    constexpr auto write(std::string_view buffer) const -> void
    { write_impl(buffer); }

    /**
     * @brief ..
     * @details ..
     */
    constexpr auto exit() -> void
    { exit_impl(); }

protected:
    /**
     * @brief ..
     * @details ..
     * @{
     */
    constexpr serial(serial const&) = default;
    constexpr serial(serial&&) = default;
    constexpr auto operator=(serial const&) -> serial & = default;
    constexpr auto operator=(serial&&) -> serial & = default;
    /** @} */

private:
    /**
     * @copydoc com::serial::setup
     */
    constexpr virtual auto setup_impl() -> void = 0;

    /**
     * @copydoc com::serial::read
     */
    constexpr virtual auto read_impl(long n_bytes) const -> std::string = 0;

    /**
     * @copydoc com::serial::write
     */
    constexpr virtual auto write_impl(std::string_view data) const -> void = 0;

    /**
     * @copydoc com::serial::exit
     */
    constexpr virtual auto exit_impl() -> void = 0;
};

} // namespace com

#endif
