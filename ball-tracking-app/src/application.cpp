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

#include <ofxCv.h>
#include <opencv.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <format>
#include <iostream>
#include <memory>
#include <numeric>
#include <vector>

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
    cam::initcamera(*camera, appcfg->cam);
    auto const framesize = appcfg->cam.frame.size(camera->getOutputBytesPerPixel());
    camframe = std::make_unique_for_overwrite<std::uint8_t[]>(framesize);
    frame = cv::Mat{
        appcfg->cam.frame.height.to<uint16>(),
        appcfg->cam.frame.width.to<uint16>(),
        CV_8UC1, camframe.get()};
    if (appcfg->serial.enabled) {
        serial->setup();
    }
    initmenu();    
}

/**
 * @brief ..
 * @details ..
 */
auto app::initmenu() noexcept -> void {
    cfgmenu.add('p', appcfg->pid.kp);
    cfgmenu.add('i', appcfg->pid.ki);
    cfgmenu.add('d', appcfg->pid.kd);

    cfgmenu.add('l', appcfg->vision.trackball);
    cfgmenu.add('z', appcfg->vision.minballradius);
    cfgmenu.add('y', appcfg->vision.maxballradius);

    cfgmenu.add('s', appcfg->serial.enabled,
        [this](auto const& cfgitem) { cfgitem ? serial->setup() : serial->exit(); });

    cfgmenu.add('h', appcfg->cam.sharpness,
        [this](auto const& cfgitem) { camera->setSharpness(cfgitem); });
    cfgmenu.add('e', appcfg->cam.exposure,
        [this](auto const& cfgitem) { camera->setExposure(cfgitem); });
    cfgmenu.add('c', appcfg->cam.contrast,
        [this](auto const& cfgitem) { camera->setContrast(cfgitem); });
    cfgmenu.add('b', appcfg->cam.brightness,
        [this](auto const& cfgitem) { camera->setBrightness(cfgitem); });

    cfgmenu.add('g', appcfg->cam.gain,
        [this](auto const& cfgitem) { camera->setGain(cfgitem); });
    cfgmenu.add('h', appcfg->cam.hue,
        [this](auto const& cfgitem) { camera->setHue(cfgitem); });

    cfgmenu.add('r', appcfg->cam.balance.red,
        [this](auto const& cfgitem) { camera->setRedBalance(cfgitem); });
    cfgmenu.add('n', appcfg->cam.balance.green,
        [this](auto const& cfgitem) { camera->setGreenBalance(cfgitem); });
    cfgmenu.add('u', appcfg->cam.balance.blue,
        [this](auto const& cfgitem) { camera->setBlueBalance(cfgitem); });

    cfgmenu.add('w', appcfg->cam.balance.autowhite,
        [this](auto const& cfgitem) { camera->setAutoWhiteBalance(cfgitem); });
    cfgmenu.add('a', appcfg->cam.autogain,
        [this](auto const& cfgitem) { camera->setAutogain(cfgitem); });
}

/**
 * @copydoc app::exit
 * @internal ..
 */
auto app::exit() -> void {
    if (not camera) return;
    camera->stop();
}

/**
 * @copydoc app::update
 * @internal ..
 */
auto app::update() -> void {
    if (not camera) return;

    camera->getFrame(camframe.get());
    updateSetPoint();
    camstats.update();
    if (appcfg->vision.trackball) {
        trackball();
    }
    if (state == appState::calibration and appcfg->serial.enabled) {
        serial->write(std::format("{:.5f} {:.5f} {:.5f} \n", 45.0, 45.0, 45.0));
    }
}

/**
 * @copydoc app::updateSetPoint
 * @internal ..
 */
auto app::updateSetPoint() -> void {
    constexpr auto cosineInterpolate = [](double y1, double y2, double mu) {
        double mu2;
        mu2 = (1 - cos(std::clamp(mu, 0.0, 1.0) * PI)) / 2;
        return (y1 * (1 - mu2) + y2 * mu2);
    };
    double t = ofGetCurrentTime().getAsSeconds() - startTime.getAsSeconds();
    t = t / moveTimeSec * 1.0;
    setPoint = oldSetPoint * (1.f - cosineInterpolate(0.0, 1.0, t))
        + newSetPoint * cosineInterpolate(0.0, 1.0, t);
    for (int i{}; i < 3; i++) {
        setPointPerAxis[i] = (setPoint.x - centerPoint.x) * transMatrices[i].x
            + (setPoint.y - centerPoint.y) * transMatrices[i].y;
    }
}

/**
 * @copydoc app::setSetPoint
 * @internal ..
 */
auto app::setSetPoint(int x, int y) -> void {
    startTime = ofGetCurrentTime();
    oldSetPoint = setPoint;
    newSetPoint = {float(x), float(y)};
    ofPoint v = newSetPoint - oldSetPoint;
    moveTimeSec = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2)) / 120.0f;
}

/**
 * @copydoc app::trackball
 * @internal ..
 */
auto app::trackball() -> void {
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1, 1000, 200, 20,
        appcfg->vision.minballradius, appcfg->vision.maxballradius);
    for (std::size_t i = 0; i < circles.size(); i++) {
        if (i == 0) {
            cv::Vec3i c = circles[i];
            cv::Point center = cv::Point(c[0], c[1]);
            cv::circle(frame, center, 1, cv::Scalar(0, 100, 100), 3, cv::LINE_AA);
            int radius = c[2];
            cv::circle(frame, center, radius, cv::Scalar(255, 0, 255), 3, cv::LINE_AA);
            if (state == appState::running) {
                ballPos = {float(center.x), float(center.y)};
                for (int j{}; j < 3; j++) {
                    ballPosPerAxis[j] = (ballPos.x - centerPoint.x) * transMatrices[j].x
                        + (ballPos.y - centerPoint.y) * transMatrices[j].y;
                }
                controlpid();
            }
        }
    }
}

/**
 * @copydoc app::pid
 * @internal ..
 */
auto app::controlpid() -> void {
    std::string output;
    for (int i{}; i < 3; i++) {
        double error = setPointPerAxis[i] - ballPosPerAxis[i];
        iError[i] += error * appcfg->pid.ki;
        iError[i] = std::clamp(iError[i], -10.0, 10.0);
        servoAction[i][servoActI] = appcfg->pid.kp * error + iError[i]
            + appcfg->pid.kd * (error - prevError[i]);
        servoAction[i][servoActI] = std::clamp(servoAction[i][servoActI], -10.0, 45.0);
        double action;
        if (error - prevError[i] < 1.75) {
            action = std::reduce(servoAction[i].begin(), servoAction[i].end()) / servoAction[i].size();
        } else {
            action = servoAction[i][servoActI];
        }
        prevError[i] = error;

        output += std::format("{:.5f} ", action + 45.0);
    }
    servoActI++;
    servoActI %= servoAction[0].size();
    output += "\n";
    std::cout << output;

    if (state == appState::running) {
        serial->write(output);
    }
}

/**
 * @copydoc app::draw
 * @internal ..
 */
auto app::draw() -> void {
    ofSetHexColor(0xffffff);
    drawcamera(0, 0);
    drawfps(10, 15);
    drawmenu(650, 150);
}

/**
 * @copydoc app::drawcamera
 * @internal ..
 */
auto app::drawcamera(float x, float y) const -> void {
    ofxCv::drawMat(frame, 0, 0, GL_R8);

    if (displayDebugVisualisation) {
        drawDebug();
    }
    switch (state) {
    case appState::calibration:
        ofDrawBitmapString(std::format(
            "Please click calibration point {}",
            pointsCalibrated + 1), 190, 200);
    }
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
 * @copydoc app::drawDebug
 * @internal ..
 */
auto app::drawDebug() const -> void {
    for (int i{0}; i < debugLines.size(); i++) {
        ofSetColor(debugLineColors[i]);
        debugLines[i].draw();
    }
    if (state == appState::running) {
        for (int i{0}; i < transMatrices.size(); i++) {

            float result = (ballPos.x - centerPoint.x) * transMatricesPreScale[i].x
                + (ballPos.y - centerPoint.y) * transMatricesPreScale[i].y;

            ofPoint v = calibrationPoints[i] - centerPoint;
            float mV = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
            ofPoint resPos = (v / mV) * result;

            ofPolyline resLine;
            resLine.addVertices({ballPos, resPos + centerPoint});
            ofColor color;
            switch (i) {
            case 0: color = {255,255,0}; break;
            case 1: color = {0,255,255}; break;
            case 2: color = {255,0,255}; break;
            }
            ofSetColor(color);
            resLine.draw();

            //Scaled output shown as sliders:

            float scaledRes = (ballPos.x - centerPoint.x) * transMatrices[i].x
                + (ballPos.y - centerPoint.y) * transMatrices[i].y;

            ofPoint displayPos{650.f, 50.f + 30.f * i};

            ofPolyline scale;
            scale.addVertices({displayPos, displayPos + ofPoint{targetScale * 2.f, 0}});
            ofPolyline pointer;
            pointer.addVertices({displayPos + ofPoint{scaledRes + targetScale, -5},
                displayPos + ofPoint{scaledRes + targetScale, 5}});
            ofPolyline center;
            center.addVertices({displayPos + ofPoint{targetScale, -3}, displayPos + ofPoint{targetScale, 3}});
            scale.draw();
            pointer.draw();
            center.draw();

            std::string str = std::format("{:.2f}", scaledRes);
            ofDrawBitmapString(str, displayPos + ofPoint{targetScale * 2.f + 10, 0});

            color.a = 128;
            ofSetColor(color);
            ofPolyline helper;
            helper.addVertices({resPos + centerPoint, centerPoint});
            helper.draw();

            ofSetColor({255, 128, 128});
            ofPolyline setpointer;
            setpointer.addVertices({displayPos + ofPoint{setPointPerAxis[i] + targetScale, -5},
                displayPos + ofPoint{setPointPerAxis[i] + targetScale, 5}});
            setpointer.draw();
        }
    }
    ofSetColor({255, 128, 128});
    ofDrawCircle(setPoint, 5.f);
    ofSetColor({255,255,255});
}

/**
 * @copydoc app::finishCalibration
 * @internal ..
 */
auto app::finishCalibration() -> void {
    centerPoint = {(calibrationPoints[0].x + calibrationPoints[1].x + calibrationPoints[2].x) / 3,
        (calibrationPoints[0].y + calibrationPoints[1].y + calibrationPoints[2].y) / 3};

    for (int i{0}; i < calibrationPoints.size(); i++) {
        int j = i + 1;
        if (j >= calibrationPoints.size()) {
            j = 0;
        }
        ofPolyline line;
        line.addVertex(centerPoint);
        line.addVertex(calibrationPoints[i]);
        debugLines.push_back(line);
        debugLineColors.push_back({0,0,150});

        ofPolyline line2;
        line2.addVertex(calibrationPoints[i]);
        line2.addVertex(calibrationPoints[j]);
        debugLines.push_back(line2);
        debugLineColors.push_back({0,200,0});
    }

    debugLines.erase(debugLines.begin(), debugLines.begin() + 3);
    debugLineColors.erase(debugLineColors.begin(), debugLineColors.begin() + 3);

    float targetSideSize = 250;
    float targetHeight = std::sqrt(std::pow(targetSideSize, 2)
        - std::pow(targetSideSize / 2.f, 2));
    std::array<ofPoint, 3> targetPoints{ofPoint{targetSideSize / 2.f, 0.f},
        ofPoint{0.f, targetHeight}, ofPoint{targetSideSize, targetHeight}};
    targetCenter = {(targetPoints[0].x + targetPoints[1].x + targetPoints[2].x) / 3.f,
        (targetPoints[0].y + targetPoints[1].y + targetPoints[2].y) / 3.f};
    targetScale = targetCenter.x;

    genTransMatrix(0);
    genTransMatrix(1);
    genTransMatrix(2);

    setPoint = {640 / 2.f, 480 / 2.f};
    setSetPoint(640 / 2, 480 / 2);

    state = appState::running;
}

/**
 * @copydoc app::genTransMatrix
 * @internal ..
 */
auto app::genTransMatrix(int i) -> void {
    ofPoint vTrans{0, 1};
    ofPoint center = centerPoint;
    ofPoint v = calibrationPoints[i] - center;

    float mV = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
    float rTrans = std::atan((v.y) / (v.x)) - std::atan(vTrans.y / vTrans.x);
    if (v.x < 0 or (v.x == 0 and v.y < 0)) {
        rTrans += M_PI;
    }
    vTrans = {std::cos(rTrans) * vTrans.x - std::sin(rTrans) * vTrans.y,
        std::sin(rTrans) * vTrans.x + std::cos(rTrans) * vTrans.y};
    transMatricesPreScale[i] = vTrans;

    transMatrices[i] = vTrans / mV * targetScale;
};

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
    case OF_KEY_TAB:     return showmenu();
    case OF_KEY_CONTROL: return reCalibrate();
    default:             return;
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
 * @copydoc app::reCalibrate
 * @internal ..
 */
auto app::reCalibrate() -> void {
    state = appState::calibration;
    debugLines.clear();
    debugLineColors.clear();
    pointsCalibrated = 0;
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
    valueprompt = std::format("{:c} | new value: ", std::toupper(key));
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
 * @copydoc app::mousePressed
 * @internal ..
 */
auto app::mousePressed(int x, int y, int button) -> void {
    switch (button) {
    case 0:
        switch (state) {
        case appState::calibration: {
            calibrationPoints[pointsCalibrated] = {float(x), float(y)};
            pointsCalibrated++;
            ofPolyline line;
            line.addVertex(ofPoint{640 / 2.f, 480 / 2.f});
            line.addVertex(ofPoint{float(x), float(y)});
            debugLines.push_back(line);
            debugLineColors.push_back({100, 0, 0});
            if (pointsCalibrated >= 3) {
                finishCalibration();
            }
            break;
        }
        case appState::running: {
            setSetPoint(x, y);
            break;
        }
        }
        break;
    }
}

/**
 * @brief ..
 * @details ..
 * @{
 */
auto app::keyReleased(int key) -> void {}
auto app::mouseMoved(int x, int y ) -> void {}
auto app::mouseDragged(int x, int y, int button) -> void {}
auto app::mouseReleased(int x, int y, int button) -> void {}
auto app::windowResized(int w, int h) -> void {}
auto app::gotMessage(ofMessage msg) -> void {}
auto app::dragEvent(ofDragInfo dragInfo) -> void {}
/** @} */

} // namespace of
