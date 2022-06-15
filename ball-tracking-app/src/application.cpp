/**
 * @file       application.cpp
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
#include "camera.h"
#include "ps3eye.h"

#include <ofxCv.h>
#include <opencv.hpp>

#include <cctype>
#include <format>
#include <iostream>
#include <memory>

/**
* @namespace ..
* @brief ..
*/
namespace of {

/**
 * @copydoc application::setup
 * @internal ..
 */
auto app::setup() -> void {
    camera = cam::getdevice();
    appcfg.loadxml();
    cam::initcamera(*camera, appcfg.cam);
    auto const framesize = appcfg.cam.frame.size(camera->getOutputBytesPerPixel());
    camframe = std::make_unique_for_overwrite<uint8[]>(framesize);
    trackframe = cv::Mat{
        appcfg.cam.frame.height.to<uint16>(),
        appcfg.cam.frame.width.to<uint16>(),
        CV_8UC3, camframe.get()};
        //CV_8UC1, camframe.get()};
    initmenu();    
}

/**
 * @brief ..
 * @details ..
 */
auto app::initmenu() noexcept -> void {
    cfgmenu.add('p', appcfg.pid.proportional);
    cfgmenu.add('i', appcfg.pid.integral);
    cfgmenu.add('d', appcfg.pid.derivative);

    cfgmenu.add('c', appcfg.vision.circlesize);

    cfgmenu.add('s', appcfg.cam.sharpness,
        [this](auto const& cfgitem) { camera->setSharpness(cfgitem); });
    cfgmenu.add('e', appcfg.cam.exposure,
        [this](auto const& cfgitem) { camera->setExposure(cfgitem); });
    cfgmenu.add('c', appcfg.cam.contrast,
        [this](auto const& cfgitem) { camera->setContrast(cfgitem); });
    cfgmenu.add('b', appcfg.cam.brightness,
        [this](auto const& cfgitem) { camera->setBrightness(cfgitem); });

    cfgmenu.add('g', appcfg.cam.gain,
        [this](auto const& cfgitem) { camera->setGain(cfgitem); });
    cfgmenu.add('h', appcfg.cam.hue,
        [this](auto const& cfgitem) { camera->setHue(cfgitem); });

    cfgmenu.add('r', appcfg.cam.balance.red,
        [this](auto const& cfgitem) { camera->setRedBalance(cfgitem); });
    cfgmenu.add('n', appcfg.cam.balance.green,
        [this](auto const& cfgitem) { camera->setGreenBalance(cfgitem); });
    cfgmenu.add('u', appcfg.cam.balance.blue,
        [this](auto const& cfgitem) { camera->setBlueBalance(cfgitem); });

    cfgmenu.add('w', appcfg.cam.balance.autowhite,
        [this](auto const& cfgitem) { camera->setAutoWhiteBalance(cfgitem); });
    cfgmenu.add('a', appcfg.cam.autogain,
        [this](auto const& cfgitem) { camera->setAutogain(cfgitem); });
}

/**
 * @copydoc app::exit
 * @internal ..
 */
auto app::exit() -> void {
    if (camera) {
        camera->stop();
    }
    appcfg.savexml();
}

/**
 * @copydoc app::update
 * @internal ..
 */
auto app::update() -> void {
    if (not camera) return;

    camera->getFrame(camframe.get());
    // trackball();
    // camstats.update();
}

/**
 * @copydoc app::trackball
 * @internal ..
 */
auto app::trackball() -> void {
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
    // std::cout << ballpos.x << ' ' << ballpos.y << std::endl;
}

/**
 * @copydoc app::draw
 * @internal ..
 */
auto app::draw() -> void {
    ofSetHexColor(0xffffff);
    drawcamera(0, 0);
    drawfps(10, 15);
    drawmenu(10, 150);
}

/**
 * @copydoc app::drawcamera
 * @internal ..
 */
auto app::drawcamera(float x, float y) const -> void {
    // Draw crosshair
    auto const color = cv::Scalar{0, 0, 255};
    // cv::drawMarker(trackframe, ballpos, color, cv::MARKER_CROSS, 50, 5);
    ofxCv::drawMat(trackframe, x, y);
    ofDrawBitmapString(std::format("ball pos: {}, {}", ballpos.x, ballpos.y),
        x + 10, y + 50);
}

/**
 * @copydoc app::drawfps
 * @internal ..
 */
auto app::drawfps(float x, float y) const -> void {
    ofDrawBitmapString(std::format(
        "app fps: {:.2f}\ncam fps: {:.2f}",
        ofGetFrameRate(), camstats.fps()), x, y);
}

/**
 * @copydoc app::drawmenu
 * @internal ..
 */
auto app::drawmenu(float x, float y) const -> void {
    ofDrawBitmapString(std::format("{}\n{}{}",
        menuprompt, valueprompt, inputvalue), x, y);
}

/**
 * @copydoc app::keyPressed
 * @internal ..
 */
auto app::keyPressed(int key) -> void {
    switch (inputmode) {
    case keyinput::app:   return handle_appinput(key);
    case keyinput::menu:  return handle_menuinput(key);
    case keyinput::value: return handle_inputvalue(key);
    default:              return;
    }
}

/**
 * @copydoc app::handle_app_input
 * @internal ..
 */
auto app::handle_appinput(int key) noexcept -> void {
    switch (key) {
    case OF_KEY_TAB: return showmenu();
    default:         return;
    }
}

/**
 * @copydoc app::showmenu
 * @internal ..
 */
auto app::showmenu() noexcept -> void {
    menuprompt = cfgmenu.to_string();
    inputmode = keyinput::menu;
}

/**
 * @copydoc app::handle_menuinput
 * @internal ..
 */
auto app::handle_menuinput(int key) noexcept -> void {
    switch (key) {
    case OF_KEY_TAB: return exitmenu();
    default:         return select_option(key);
    }
}

/**
 * @copydoc app::exitmenu
 * @internal ..
 */
auto app::exitmenu() noexcept -> void {
    menuprompt.clear();
    inputmode = keyinput::app;
}

/**
 * @copydoc app::select_option
 * @internal ..
 */
auto app::select_option(int key) noexcept -> void {
    if (not cfgmenu.contains(key)) return;

    cfgmenu.select(key);
    valueprompt = std::format("{:c} / new value: ", std::toupper(key));
    inputmode = keyinput::value;
}

/**
 * @copydoc app::handle_inputvalue
 * @internal ..
 */
auto app::handle_inputvalue(int key) -> void {
    switch (key) {
    case OF_KEY_RETURN:    return apply_inputvalue();
    case OF_KEY_BACKSPACE: return erase_inputvalue();
    default:               return add_inputvalue(key);
    }
}

/**
 * @copydoc app::apply_inputvalue
 * @internal ..
 */
auto app::apply_inputvalue() -> void {
    if (inputvalue.empty()) return;

    cfgmenu.selection().value().apply(inputvalue);
    menuprompt = cfgmenu.to_string();
    valueprompt.clear();
    inputvalue.clear();
    inputmode = keyinput::menu;
}

/**
 * @copydoc app::erase_inputvalue
 * @internal ..
 */
auto app::erase_inputvalue() noexcept -> void {
    if (inputvalue.empty()) return;
    inputvalue.pop_back();
}

/**
 * @copydoc app::add_inputvalue
 * @internal ..
 */
auto app::add_inputvalue(unsigned char key) noexcept -> void {
    if (not (std::isdigit(key) or key == '.')) return;
    inputvalue += key;
}

/**
 * @brief ..
 * @details ..
 * @{
 */
auto app::keyReleased(int key) -> void {}
auto app::mouseMoved(int x, int y ) -> void {}
auto app::mouseDragged(int x, int y, int button) -> void {}
auto app::mousePressed(int x, int y, int button) -> void {}
auto app::mouseReleased(int x, int y, int button) -> void {}
auto app::windowResized(int w, int h) -> void {}
auto app::gotMessage(ofMessage msg) -> void {}
auto app::dragEvent(ofDragInfo dragInfo) -> void {}
/** @} */

} // namespace of
