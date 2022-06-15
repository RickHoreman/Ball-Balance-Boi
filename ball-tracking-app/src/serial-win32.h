/**
 * @file       serial-win32.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief ..
 * @details ..
 */

#ifndef COMM_SERIAL_WIN32_H
#define COMM_SERIAL_WIN32_H

#include "serial.h"

#include <windows.h>

#include <string>
#include <string_view>
#include <utility>

/**
 * @namespace ..
 * @brief ..
 */
namespace comm {

/**
 * @class ..
 * @brief ..
 * @details ..
 */
class serial_win32 : public serial {
public:
    /**
     * @brief ..
     */
    serial_win32() = default;

    /**
     * @brief ..
     * @details ..
     * @param[in] portname ..
     * @param[in] baudrate ..
     */
    constexpr explicit serial_win32(std::string portname, long baudrate = CBR_9600)
        : portname_{std::move(portname)}, baudrate_{baudrate} {}

    /**
    * @brief ..
    * @return ..
    */
    constexpr auto portname() const noexcept -> std::string
    { return portname_; }

    /**
     * @brief ..
     * @return ..
     */
    constexpr auto baudrate() const noexcept -> long
    { return baudrate_; }

private:
    /**
     * @copydoc com::serial::setup
     * @internal ..
     */
    auto setup_impl() -> void override {
        open_commport();
        init_commdevice();
    }

    /**
     * @brief ..
     * @details ..
     */
    auto open_commport() -> void {
        auto portpath = L"\\\\.\\" + std::wstring{portname_.begin(), portname_.end()};
        commport = CreateFile(portpath.data(),
            GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
        if (commport == INVALID_HANDLE_VALUE) {
            throw serial_error{"unable to open serial port: " + portname_};
        }
    }

    /**
     * @brief ..
     * @details ..
     */
    auto init_commdevice() -> void {
        DCB config;
        if (not GetCommState(commport, &config)) {
            throw serial_error{"unable to obtain serial parameters"};
        }
        config.DCBlength = sizeof(DCB);
        config.BaudRate  = baudrate_;
        config.ByteSize  = 8;
        config.StopBits  = ONESTOPBIT;
        config.Parity    = PARITY_NONE;
        if (not SetCommState(commport, &config)) {
            throw serial_error{"unable to configure serial parameters"};
        }
        PurgeComm(commport, PURGE_RXCLEAR | PURGE_TXCLEAR);
    }

    /**
     * @copydoc com::serial::read
     * @internal ..
     */
    auto read_impl(long n_bytes) const -> std::string override {
        std::string buffer;
        buffer.reserve(n_bytes);
        DWORD bytesread;
        if (not ReadFile(commport, buffer.data(), n_bytes, &bytesread, nullptr)) {
            throw serial_error{"unable to read from serial port: " + portname_};
        }
        return buffer;
    }

    /**
     * @copydoc com::serial::write
     * @internal ..
     */
    auto write_impl(std::string_view buffer) const -> void override {
        DWORD bytesread;
        if (not WriteFile(commport, buffer.data(), buffer.size(), &bytesread, nullptr)) {
            throw serial_error{"unable to write to serial port: " + portname_};
        }
    }

    /**
     * @copydoc com::serial::exit
     * @internal ..
     */
    auto exit_impl() -> void override
    { CloseHandle(commport); }

    std::string portname_;
    HANDLE commport{};
    long baudrate_{CBR_9600};
};

} // namespace com

#endif
