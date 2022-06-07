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

#include <ofxCv.h>
#include <opencv.hpp>
#include <tracking.hpp> // needed?

#include <cctype>
#include <charconv>
#include <exception>
#include <format>
#include <iostream>
#include <memory>

/**
 * @copydoc ofApp::setup
 * @internal ..
 */
auto ofApp::setup() -> void {
    if (camera = cam::getdevice(); not camera) {
        throw std::runtime_error("could not find ps3 camera");
    }
    if (cam::initcamera(*camera, camcfg) != cam::status::operational) {
        throw std::runtime_error("could not initialize ps3 camera");
    }
    camframe = std::make_unique_for_overwrite<std::uint8_t[]>(camcfg.frame.size(3));
    initmenu();
    ofSetFrameRate(80);
}

auto ofApp::initmenu() noexcept -> void {
    inputmenu.add('s', "sharpness",
        +[](cam::ps3cam const& camera)
        { return camera.getSharpness(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setSharpness(value); });
    inputmenu.add('e', "exposure",
        +[](cam::ps3cam const& camera)
        { return camera.getExposure(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setExposure(value); });
    inputmenu.add('b', "brightness",
        +[](cam::ps3cam const& camera)
        { return camera.getBrightness(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setBrightness(value); });
    inputmenu.add('c', "contrast",
        +[](cam::ps3cam const& camera)
        { return camera.getContrast(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setContrast(value); });

    inputmenu.add('g', "gain",
        +[](cam::ps3cam const& camera)
        { return camera.getGain(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setGain(value); });
    inputmenu.add('h', "hue",
        +[](cam::ps3cam const& camera)
        { return camera.getHue(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setHue(value); });

    inputmenu.add('d', "red balance",
        +[](cam::ps3cam const& camera)
        { return camera.getRedBalance(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setRedBalance(value); });
    inputmenu.add('n', "green balance",
        +[](cam::ps3cam const& camera)
        { return camera.getGreenBalance(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setGreenBalance(value); });
    inputmenu.add('u', "blue balance",
        +[](cam::ps3cam const& camera)
        { return camera.getBlueBalance(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setBlueBalance(value); });

    inputmenu.add('w', "auto white bal.",
        +[](cam::ps3cam const& camera) -> std::uint8_t
        { return camera.getAutoWhiteBalance(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setAutoWhiteBalance(value); });
    inputmenu.add('a', "auto gain",
        +[](cam::ps3cam const& camera) -> std::uint8_t
        { return camera.getAutogain(); },
        +[](cam::ps3cam& camera, std::uint8_t value)
        { camera.setAutogain(value); });
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
    // cv::Mat frame{camcfg.frame.height, camcfg.frame.width, CV_8UC1, camframe.get()};
    cv::Mat frame{camcfg.frame.width, camcfg.frame.height, CV_8UC3, camframe.get()};

    // Resize large images to reduce processing load
    // cap >> frame;

    // Convert RGB to HSV colormap and apply Gaussain blur
    cv::Mat hsvFrame;
    cv::cvtColor(frame, hsvFrame, CV_RGB2HSV);
    // cv::cvtColor(frame, hsvFrame, CV_BGR2HSV);

    cv::blur(hsvFrame, hsvFrame, cv::Size(1, 1));

    // Threshold 
    // Scalar lowerBound = cv::Scalar(55, 100, 50);
    // Scalar upperBound = cv::Scalar(90, 255, 255);
    cv::Scalar lowerBound = cv::Scalar(0, 100, 50);
    cv::Scalar upperBound = cv::Scalar(50, 255, 255);
    cv::Mat threshFrame;
    cv::inRange(hsvFrame, lowerBound, upperBound, threshFrame);

    // Calculate X,Y centroid
    cv::Moments m = moments(threshFrame, false);
    cv::Point com(m.m10 / m.m00, m.m01 / m.m00);

    // Draw crosshair
    cv::Scalar color = cv::Scalar(0, 0, 255);
    cv::drawMarker(frame, com, color, cv::MARKER_CROSS, 50, 5);
    // ofxCv::toOf(frame, colorimg);
    ofxCv::drawMat(frame, 0, 0);
    // ofxCv::drawMat(threshFrame, 600, 400);

    ofDrawBitmapString(std::format(
        "ball pos: {}, {}", com.x, com.y), 10, 50);

    std::cout << com.x << ' ' << com.y << std::endl;
}

/**
 * @copydoc ofApp::draw
 * @internal ..
 */
auto ofApp::draw() -> void {
    ofSetHexColor(0xffffff);
    drawfps(10, 15);
    drawmenu(10, 150);
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

auto ofApp::keyReleased(int key) -> void{}
auto ofApp::mouseMoved(int x, int y ) -> void {}
auto ofApp::mouseDragged(int x, int y, int button) -> void {}
auto ofApp::mousePressed(int x, int y, int button) -> void {}
auto ofApp::mouseReleased(int x, int y, int button) -> void {}
auto ofApp::windowResized(int w, int h) -> void {}
auto ofApp::gotMessage(ofMessage msg) -> void {}
auto ofApp::dragEvent(ofDragInfo dragInfo) -> void {}
