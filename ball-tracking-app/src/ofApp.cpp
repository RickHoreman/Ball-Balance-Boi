/**
 * @file       ofApp.h
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
#include "ps3eye.h"
#include "camera.h"

#include <windows.h>

#include <ofxCv.h>
#include <opencv.hpp>
#include <tracking.hpp> // needed?

#include <cctype>
#include <charconv>
#include <format>
#include <iostream>
#include <memory>
#include <stdexcept>

/**
 * @copydoc ofApp::setup
 * @internal ..
 */
auto ofApp::setup() -> void {
    if (camera = cam::getdevice(); camera == nullptr) {
        throw std::runtime_error("could not find ps3 camera");
    }
    if (cam::initcamera(*camera, camcfg) != cam::status::operational) {
        throw std::runtime_error("could not initialize ps3 camera");
    }
    auto const framesize = camcfg.frame.size(camera->getOutputBytesPerPixel());
    camframe = std::make_unique_for_overwrite<std::uint8_t[]>(framesize);
    trackframe = cv::Mat{camcfg.frame.height, camcfg.frame.width,
        CV_8UC3, camframe.get()};
        // CV_8UC1, camframe.get()};
    ofSetFrameRate(75);
    initmenu();
    //
}

auto ofApp::initmenu() noexcept -> void {
    using ps3cam = cam::ps3cam;

    inputmenu.add('s', "sharpness",
        opt::dispatch<&ps3cam::getSharpness, &ps3cam::setSharpness>());
    inputmenu.add('e', "exposure",
        opt::dispatch<&ps3cam::getExposure, &ps3cam::setExposure>());
    inputmenu.add('c', "contrast",
        opt::dispatch<&ps3cam::getContrast, &ps3cam::setContrast>());
    inputmenu.add('b', "brightness",
        opt::dispatch<&ps3cam::getBrightness, &ps3cam::setBrightness>());

    inputmenu.add('g', "gain",
        opt::dispatch<&ps3cam::getGain, &ps3cam::setGain>());
    inputmenu.add('h', "hue",
        opt::dispatch<&ps3cam::getHue, &ps3cam::setHue>());

    inputmenu.add('d', "red balance",
        opt::dispatch<&ps3cam::getRedBalance, &ps3cam::setRedBalance>());
    inputmenu.add('n', "green balance",
        opt::dispatch<&ps3cam::getGreenBalance, &ps3cam::setGreenBalance>());
    inputmenu.add('u', "blue balance",
        opt::dispatch<&ps3cam::getBlueBalance, &ps3cam::setBlueBalance>());

    inputmenu.add('w', "auto white bal.", opt::dispatch<
        &ps3cam::getAutoWhiteBalance, &ps3cam::setAutoWhiteBalance, std::uint8_t>());
    inputmenu.add('a', "auto gain",
        opt::dispatch<&ps3cam::getAutogain, &ps3cam::setAutogain, std::uint8_t>());
}

/**
 * @copydoc ofApp::exit
 * @internal ..
 */
auto ofApp::exit() -> void {
    if (camera) {
        camera->stop();
    }
    // camframe.reset();
}

/**
 * @copydoc ofApp::update
 * @internal ..
 */
auto ofApp::update() -> void {
    if (not camera) return;

    camera->getFrame(camframe.get());
    trackball();
    // camstats.update();
}

/**
 * @copydoc ofApp::trackball
 * @internal ..
 */
auto ofApp::trackball() -> void {
    // Convert RGB to HSV colormap and apply Gaussain blur
    cv::Mat hsvframe;
    // cv::cvtColor(trackframe, hsvFrame, CV_BGR2HSV);
    cv::cvtColor(trackframe, hsvframe, CV_RGB2HSV);
    cv::blur(hsvframe, hsvframe, cv::Size{1, 1});

    // Threshold 
    // auto const lowerbound = cv::Scalar{55, 100, 50};
    // auto const upperbound = cv::Scalar{90, 255, 255};
    auto const lowerbound = cv::Scalar{0, 100, 50};
    auto const upperbound = cv::Scalar{50, 255, 255};
    cv::Mat threshframe;
    cv::inRange(hsvframe, lowerbound, upperbound, threshframe);

    // Calculate X,Y centroid
    auto const ball = cv::moments(threshframe, false);
    ballpos.x = ball.m10 / ball.m00;
    ballpos.y = ball.m01 / ball.m00;

    // 'send' the location of the ball 
    std::cout << ballpos.x << ' ' << ballpos.y << std::endl;
}

/**
 * @copydoc ofApp::draw
 * @internal ..
 */
auto ofApp::draw() -> void {
    ofSetHexColor(0xffffff);
    drawcamera(0, 0);
    drawfps(10, 15);
    drawmenu(10, 150);
}

/**
 * @copydoc ofApp::drawcamera
 * @internal ..
 */
auto ofApp::drawcamera(float x, float y) const -> void {
    // Draw crosshair
    auto const color = cv::Scalar{0, 0, 255};
    cv::drawMarker(trackframe, ballpos, color, cv::MARKER_CROSS, 50, 5);
    ofxCv::drawMat(trackframe, x, y);
    ofDrawBitmapString(std::format("ball pos: {}, {}", ballpos.x, ballpos.y),
        x + 10, y + 50);
}

/**
 * @copydoc ofApp::drawfps
 * @internal ..
 */
auto ofApp::drawfps(float x, float y) const -> void {
    ofDrawBitmapString(std::format(
        "app fps: {:.2f}\ncam fps: {:.2f}",
        ofGetFrameRate(), camstats.fps()), x, y);
}

/**
 * @copydoc ofApp::drawmenu
 * @internal ..
 */
auto ofApp::drawmenu(float x, float y) const -> void {
    ofDrawBitmapString(std::format("{}\n{}{}",
        menuprompt, inputprompt, inputvalue), x, y);
}

/**
 * @copydoc ofApp::keyPressed
 * @internal ..
 */
auto ofApp::keyPressed(int key) -> void {
    switch (inputmode) {
    case keyinput::app:   return handle_app_input(key);
    case keyinput::menu:  return handle_menu_input(key);
    case keyinput::value: return handle_input_value(key);
    default:              return;
    }
}

/**
 * @copydoc ofApp::handle_app_input
 * @internal ..
 */
auto ofApp::handle_app_input(int key) noexcept -> void {
    switch (key) {
    case OF_KEY_TAB: return show_menu();
    default:         return;
    }
}

/**
 * @copydoc ofApp::show_menu
 * @internal ..
 */
auto ofApp::show_menu() noexcept -> void {
    menuprompt = inputmenu.to_string(*camera);
    inputmode = keyinput::menu;
}

/**
 * @copydoc ofApp::handle_menu_input
 * @internal ..
 */
auto ofApp::handle_menu_input(int key) noexcept -> void {
    switch (key) {
    case OF_KEY_TAB: return exit_menu();
    default:         return select_option(key);
    }
}

/**
 * @copydoc ofApp::exit_menu
 * @internal ..
 */
auto ofApp::exit_menu() noexcept -> void {
    menuprompt.clear();
    inputmode = keyinput::app;
}

/**
 * @copydoc ofApp::exit_menu
 * @internal ..
 */
auto ofApp::select_option(int key) noexcept -> void {
    if (not inputmenu.contains(key)) return;

    inputmenu.select(key);
    inputprompt = "new value: ";
    inputmode = keyinput::value;
}

/**
 * @copydoc ofApp::handle_input_value
 * @internal ..
 */
auto ofApp::handle_input_value(int key) -> void {
    switch (key) {
    case OF_KEY_RETURN:    return apply_input_value();
    case OF_KEY_BACKSPACE: return erase_input_value();
    default:               return add_input_value(key);
    }
}

/**
 * @copydoc ofApp::apply_input_value
 * @internal ..
 */
auto ofApp::apply_input_value() -> void {
    if (inputvalue.empty()) return;

    auto result = std::uint8_t{255};
    auto const status = std::from_chars(inputvalue.data(),
        inputvalue.data() + inputvalue.size(), result);
    inputmenu.selection().setvalue(*camera, result);
    menuprompt = inputmenu.to_string(*camera);
    inputprompt.clear();
    inputvalue.clear();
    inputmode = keyinput::menu;
}

/**
 * @copydoc ofApp::erase_input_value
 * @internal ..
 */
auto ofApp::erase_input_value() noexcept -> void {
    if (inputvalue.empty()) return;
    inputvalue.pop_back();
}

/**
 * @copydoc ofApp::add_input_value
 * @internal ..
 */
auto ofApp::add_input_value(unsigned char key) noexcept -> void {
    if (not std::isdigit(key)) return;
    inputvalue += key;
}

auto ofApp::keyReleased(int key) -> void {}
auto ofApp::mouseMoved(int x, int y ) -> void {}
auto ofApp::mouseDragged(int x, int y, int button) -> void {}
auto ofApp::mousePressed(int x, int y, int button) -> void {}
auto ofApp::mouseReleased(int x, int y, int button) -> void {}
auto ofApp::windowResized(int w, int h) -> void {}
auto ofApp::gotMessage(ofMessage msg) -> void {}
auto ofApp::dragEvent(ofDragInfo dragInfo) -> void {}
