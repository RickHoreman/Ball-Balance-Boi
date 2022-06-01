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
		auto app = ofApp{};
		ofRunApp(&app);
	} catch (std::exception const& error) {
		std::cerr << "unexpected exception occurred: " << error.what();
		return EXIT_FAILURE;
	} catch (...) {
		std::cerr << "unhandled exception occurred";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
