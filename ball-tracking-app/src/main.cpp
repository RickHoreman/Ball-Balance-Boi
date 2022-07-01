/**
 * @file       main.cpp
 * @version    0.1
 * @date       May 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief Entry point for the ball-tracking application.
 * @details This application is capable of positioning a ball to a user-defined setpoint
 *     on a flat plate that can be tilted along 3 separate axes. The position of the ball
 *     is determined by analyzing the camera feed of a PS3 Eye webcam. Ball positions are
 *     translated to angles and sent to a servo controller that can tilt the plate
 *     accordingly.
 */

#include "application.h"

#include <ofMain.h>

#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>

/**
 * @brief Starts running this application.
 * @details Attempts to load a configuration file in the current directory called
 *     "settings.xml" by default. The active settings are saved upon closing.
 * @retval EXIT_SUCCESS The application exited gracefully.
 * @retval EXIT_FAILURE Some unforeseen exception occurred.
 */
auto main() -> int {
    try {
        ofDisableDataPath();
        auto appcfg = cfg::config::defaults();
        appcfg.loadxml();
        ofSetupOpenGL(appcfg.screen.width, appcfg.screen.height, OF_WINDOW);
        ofSetFrameRate(appcfg.screen.rate);
        ofRunApp(new of::app{appcfg});
        appcfg.savexml();
        return EXIT_SUCCESS;
    } catch (std::exception const& error) {
        std::cerr << std::format(
            "unexpected exception occurred: {}\n", error.what());
    } catch (...) {
        std::cerr << "unhandled exception occurred\n";
    }
    return EXIT_FAILURE;
}
