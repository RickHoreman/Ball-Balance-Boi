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

#include "ofApp.h"
#include "serial-win32.h"

#include <ofMain.h>

#include <cstdlib>
#include <exception>
#include <iostream>

/**
 * @brief ..
 * @return ..
 */
auto main() -> int {
	struct {
		int width;
		int height;
	} constexpr screen{1920, 1080};
	ofSetupOpenGL(screen.width, screen.height, OF_WINDOW);

	try {
		auto serial = comm::serial_win32{"COM6", 115'200};
		ofRunApp(new ofApp{serial});
		return EXIT_SUCCESS;
	} catch (std::exception const& error) {
		std::cerr << "unexpected exception occurred: " << error.what();
	} catch (...) {
		std::cerr << "unhandled exception occurred";
	}
	return EXIT_FAILURE;
}
