/**
 * @file       main.cpp
 * @version    0.1
 * @date       May 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief ..
 * @details ..
 */

#include "application.h"
#include "serial-win32.h"

#include <ofMain.h>

#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>

/**
 * @brief ..
 * @return ..
 */
auto main() -> int {
    try {
        ofDisableDataPath();
        auto appcfg = cfg::config::defaults();
        appcfg.loadxml();
        ofSetupOpenGL(appcfg.screen.width, appcfg.screen.height, OF_WINDOW);
        ofSetFrameRate(appcfg.screen.rate);
        auto serial = comm::serial_win32{
            std::format("COM{}", appcfg.serial.comport),
            appcfg.serial.baudrate.to<long>()};
        ofRunApp(new of::app{appcfg, serial});
        appcfg.savexml();
        return EXIT_SUCCESS;
    } catch (std::exception const& error) {
        std::cerr << std::format("unexpected exception occurred: {}\n", error.what());
    } catch (...) {
        std::cerr << "unhandled exception occurred\n";
    }
    return EXIT_FAILURE;
}
