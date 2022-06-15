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
		ofSetupOpenGL(1920, 1080, OF_WINDOW);
		ofSetFrameRate(75);
		ofDisableDataPath();
		auto serial = comm::serial_win32{"COM6", 115'200};
		ofRunApp(new of::app{serial});
		return EXIT_SUCCESS;
	} catch (std::exception const& error) {
		std::cerr << std::format("unexpected exception occurred: {}\n", error.what());
	} catch (...) {
		std::cerr << "unhandled exception occurred\n";
	}
	return EXIT_FAILURE;
}
