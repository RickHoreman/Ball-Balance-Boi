/**
 * @file       application.cpp
 * @version    0.1
 * @date       May 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief Implementation of the user interface ball-tracking application.
 */

#include "application.h"

#include <ofxCv.h>
#include <opencv.hpp>

#include <algorithm>
#include <cctype>
#include <format>
#include <memory>
#include <numbers>
#include <numeric>
#include <vector>

/**
 * @namespace of
 * @brief OpenFrameworks related components.
 */
namespace of {

/**
 * @copydoc app::setup
 */
auto app::setup() -> void {
    camera = cam::get_device();
    cam::start_camera(*camera, appcfg->cam);
    auto const framesize = appcfg->cam.frame.size(camera->getOutputBytesPerPixel());
    camframe = std::make_unique_for_overwrite<std::uint8_t[]>(framesize);
    frame = cv::Mat{
        appcfg->cam.frame.height.to<int>(),
        appcfg->cam.frame.width.to<int>(),
        CV_8UC1, camframe.get()};
    ballradius.min = appcfg->vision.ballradius.min;
    ballradius.max = appcfg->vision.ballradius.max;
    pid.kp = appcfg->pid.kp;
    pid.ki = appcfg->pid.ki;
    pid.kd = appcfg->pid.kd;
    if (appcfg->serial.enabled) {
        start_serial();
    }
    make_menu();
}

/**
 * @copydoc app::start_serial
 */
auto app::start_serial() -> void {
    if (serial.setup(appcfg->serial.deviceid, appcfg->serial.baudrate)) return;
    throw serial_error{"unable to open serial device #"
        + appcfg->serial.deviceid.to<std::string>()};
}

/**
 * @copydoc app::make_menu
 */
auto app::make_menu() noexcept -> void {
    cfgmenu.add('p', appcfg->pid.kp, [this]{ pid.kp = appcfg->pid.kp; });
    cfgmenu.add('i', appcfg->pid.ki, [this]{ pid.ki = appcfg->pid.ki; });
    cfgmenu.add('d', appcfg->pid.kd, [this]{ pid.kd = appcfg->pid.kd; });

    cfgmenu.add('v', appcfg->vision.displaydebug);
    cfgmenu.add('l', appcfg->vision.trackball);
    cfgmenu.add('z', appcfg->vision.ballradius.min,
        [this]{ ballradius.min = appcfg->vision.ballradius.min; });
    cfgmenu.add('y', appcfg->vision.ballradius.max,
        [this]{ ballradius.max = appcfg->vision.ballradius.max; });

    cfgmenu.add('s', appcfg->serial.enabled,
        [this]{ appcfg->serial.enabled ? start_serial() : serial.close(); });

    cfgmenu.add('h', appcfg->cam.sharpness,
        [this]{ camera->setSharpness(appcfg->cam.sharpness); });
    cfgmenu.add('e', appcfg->cam.exposure,
        [this]{ camera->setExposure(appcfg->cam.exposure); });
    cfgmenu.add('c', appcfg->cam.contrast,
        [this]{ camera->setContrast(appcfg->cam.contrast); });
    cfgmenu.add('b', appcfg->cam.brightness,
        [this]{ camera->setBrightness(appcfg->cam.brightness); });

    cfgmenu.add('g', appcfg->cam.gain, [this]{ camera->setGain(appcfg->cam.gain); });
    cfgmenu.add('h', appcfg->cam.hue,  [this]{ camera->setHue(appcfg->cam.hue); });

    cfgmenu.add('r', appcfg->cam.balance.red,
        [this]{ camera->setRedBalance(appcfg->cam.balance.red); });
    cfgmenu.add('n', appcfg->cam.balance.green,
        [this]{ camera->setGreenBalance(appcfg->cam.balance.green); });
    cfgmenu.add('u', appcfg->cam.balance.blue,
        [this]{ camera->setBlueBalance(appcfg->cam.balance.blue); });

    cfgmenu.add('w', appcfg->cam.balance.autowhite,
        [this]{ camera->setAutoWhiteBalance(appcfg->cam.balance.autowhite); });
    cfgmenu.add('a', appcfg->cam.autogain,
        [this]{ camera->setAutogain(appcfg->cam.autogain); });
}

/**
 * @copydoc app::exit
 */
auto app::exit() -> void {
    if (not camera) return;
    camera->stop();
}

/**
 * @copydoc app::update
 */
auto app::update() -> void {
    if (not camera) return;

    camera->getFrame(camframe.get());
    updateSetPoint();
    camstats.update();
    if (appcfg->vision.trackball) {
        track_ball();
    }
    if (appmode == appstate::calibration and appcfg->serial.enabled) {
        constexpr auto servopos = std::string_view{"45.0 45.0 45.0 \n"};
        serial.writeBytes(servopos.data(), servopos.size());
    }
}

/**
 * @copydoc app::updateSetPoint
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
 */
auto app::setSetPoint(int x, int y) -> void {
    startTime = ofGetCurrentTime();
    oldSetPoint = setPoint;
    newSetPoint = {float(x), float(y)};
    ofPoint v = newSetPoint - oldSetPoint;
    moveTimeSec = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2)) / 120.0f;
}

/**
 * @copydoc app::track_ball
 */
auto app::track_ball() -> void {
    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1, 1000, 200, 20,
        ballradius.min, ballradius.max);
    if (circles.size() == 0) return;
    cv::Vec3i c = circles[0];
    cv::Point center = cv::Point(c[0], c[1]);
    cv::circle(frame, center, 1, cv::Scalar(0, 100, 100), 3, cv::LINE_AA);
    int radius = c[2];
    cv::circle(frame, center, radius, cv::Scalar(255, 0, 255), 3, cv::LINE_AA);
    if (appmode == appstate::running) {
        ballPos = {float(center.x), float(center.y)};
        for (int j{}; j < 3; j++) {
            ballPosPerAxis[j] = (ballPos.x - centerPoint.x) * transMatrices[j].x
                + (ballPos.y - centerPoint.y) * transMatrices[j].y;
        }
        control_pid();
    }
}

/**
 * @copydoc app::control_pid
 */
auto app::control_pid() -> void {
    std::string output;
    for (int i{}; i < 3; i++) {
        double error = setPointPerAxis[i] - ballPosPerAxis[i];
        iError[i] += error * pid.ki;
        iError[i] = std::clamp(iError[i], -10.0, 10.0);
        servoAction[i][servoActI] = pid.kp * error + iError[i]
            + pid.kd * (error - prevError[i]);
        servoAction[i][servoActI] = std::clamp(servoAction[i][servoActI], -10.0, 45.0);
        double action;
        if (error - prevError[i] < 1.75) {
            action = std::reduce(
                servoAction[i].begin(), servoAction[i].end()) / servoAction[i].size();
        } else {
            action = servoAction[i][servoActI];
        }
        prevError[i] = error;
        output += std::format("{:.5f} ", action + 45.0);
    }
    servoActI++;
    servoActI %= servoAction[0].size();
    output += "\n";

    if (appmode == appstate::running and appcfg->serial.enabled) {
        serial.writeBytes(output.data(), output.size());
    }
}

/**
 * @copydoc app::draw
 */
auto app::draw() -> void {
    ofSetHexColor(0xffffff);
    draw_camera(0, 0);
    draw_fps(10, 15);
    draw_menu(650, 150);
}

/**
 * @copydoc app::draw_camera
 */
auto app::draw_camera(float x, float y) const -> void {
    ofxCv::drawMat(frame, 0, 0, GL_R8);

    if (appcfg->vision.displaydebug) {
        draw_debug();
    }
    switch (appmode) {
    case appstate::calibration:
        return ofDrawBitmapString(std::format(
            "Please click calibration point {}",
            pointsCalibrated + 1), 190, 200);
    default:
        return;
    }
}

/**
 * @copydoc app::draw_fps
 */
auto app::draw_fps(float x, float y) const -> void {
    ofDrawBitmapString(std::format(
        "app fps: {:.2f}\ncam fps: {:.2f}",
        ofGetFrameRate(), camstats.fps()), x, y);
}

/**
 * @copydoc app::draw_menu
 */
auto app::draw_menu(float x, float y) const -> void {
    ofDrawBitmapString(std::format("{}\n{}{}",
        menuprompt, valueprompt, inputvalue), x, y);
}

/**
 * @copydoc app::draw_debug
 */
auto app::draw_debug() const -> void {
    for (int i{0}; i < debugLines.size(); i++) {
        ofSetColor(debugLineColors[i]);
        debugLines[i].draw();
    }
    if (appmode == appstate::running) {
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

            // Scaled output shown as sliders:

            float scaledRes = (ballPos.x - centerPoint.x) * transMatrices[i].x
                + (ballPos.y - centerPoint.y) * transMatrices[i].y;

            ofPoint displayPos{650.f, 50.f + 30.f * i};

            ofPolyline scale;
            scale.addVertices({displayPos, displayPos + ofPoint{targetScale * 2.f, 0}});
            ofPolyline pointer;
            pointer.addVertices({displayPos + ofPoint{scaledRes + targetScale, -5},
                displayPos + ofPoint{scaledRes + targetScale, 5}});
            ofPolyline center;
            center.addVertices({displayPos + ofPoint{targetScale, -3},
                displayPos + ofPoint{targetScale, 3}});
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
            setpointer.addVertices(
                {displayPos + ofPoint{setPointPerAxis[i] + targetScale, -5},
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
 */
auto app::finishCalibration() -> void {
    centerPoint = {
        (calibrationPoints[0].x + calibrationPoints[1].x + calibrationPoints[2].x) / 3,
        (calibrationPoints[0].y + calibrationPoints[1].y + calibrationPoints[2].y) / 3
    };
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

    appmode = appstate::running;
}

/**
 * @copydoc app::genTransMatrix
 */
auto app::genTransMatrix(int axis) -> void {
    ofPoint vTrans{0, 1};
    ofPoint center = centerPoint;
    ofPoint v = calibrationPoints[axis] - center;

    float mV = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
    float rTrans = std::atan((v.y) / (v.x)) - std::atan(vTrans.y / vTrans.x);
    if (v.x < 0 or (v.x == 0 and v.y < 0)) {
        rTrans += std::numbers::pi;
    }
    vTrans = {std::cos(rTrans) * vTrans.x - std::sin(rTrans) * vTrans.y,
        std::sin(rTrans) * vTrans.x + std::cos(rTrans) * vTrans.y};
    transMatricesPreScale[axis] = vTrans;
    transMatrices[axis] = vTrans / mV * targetScale;
};

/**
 * @copydoc app::keyPressed
 */
auto app::keyPressed(int key) -> void {
    switch (inputmode) {
    case inputstate::app:   return handle_key_event(key);
    case inputstate::menu:  return handle_menu_event(key);
    case inputstate::value: return handle_input_event(key);
    default:                return;
    }
}

/**
 * @copydoc app::handle_key_event
 */
auto app::handle_key_event(int key) noexcept -> void {
    switch (key) {
    case OF_KEY_TAB:     return show_menu();
    case OF_KEY_CONTROL: return recalibrate();
    default:             return;
    }
}

/**
 * @copydoc app::show_menu
 */
auto app::show_menu() noexcept -> void {
    menuprompt = cfgmenu.to<std::string>();
    inputmode = inputstate::menu;
}

/**
 * @copydoc app::recalibrate
 */
auto app::recalibrate() -> void {
    debugLines.clear();
    debugLineColors.clear();
    pointsCalibrated = 0;
    appmode = appstate::calibration;
}

/**
 * @copydoc app::handle_menu_event
 */
auto app::handle_menu_event(int key) noexcept -> void {
    switch (key) {
    case OF_KEY_TAB: return exit_menu();
    default:         return select_option(key);
    }
}

/**
 * @copydoc app::exit_menu
 */
auto app::exit_menu() noexcept -> void {
    menuprompt.clear();
    inputmode = inputstate::app;
}

/**
 * @copydoc app::select_option
 */
auto app::select_option(unsigned char key) noexcept -> void {
    if (not cfgmenu.select(key)) return;

    valueprompt = std::format("{:c} | new value: ", std::toupper(key));
    inputmode = inputstate::value;
}

/**
 * @copydoc app::handle_input_event
 */
auto app::handle_input_event(int key) -> void {
    switch (key) {
    case OF_KEY_RETURN:    return apply_input_value();
    case OF_KEY_BACKSPACE: return erase_input_value();
    default:               return add_input_value(key);
    }
}

/**
 * @copydoc app::apply_input_value
 */
auto app::apply_input_value() -> void {
    if (inputvalue.empty()) return;

    cfgmenu.selection().apply(inputvalue);
    menuprompt = cfgmenu.to<std::string>();
    valueprompt.clear();
    inputvalue.clear();
    inputmode = inputstate::menu;
}

/**
 * @copydoc app::erase_input_value
 */
auto app::erase_input_value() noexcept -> void {
    if (inputvalue.empty()) return;
    inputvalue.pop_back();
}

/**
 * @copydoc app::add_input_value
 */
auto app::add_input_value(unsigned char key) noexcept -> void {
    if (not (std::isdigit(key) or key == '.')) return;
    inputvalue += key;
}

/**
 * @copydoc app::mousePressed
 */
auto app::mousePressed(int x, int y, int button) -> void {
    switch (button) {
    case 0:  return handle_mouse_event(x, y);
    default: return;
    }
}

/**
 * @copydoc app::handle_mouse_event
 */
auto app::handle_mouse_event(int x, int y) -> void {
    switch (appmode) {
    case appstate::running:     return setSetPoint(x, y);
    case appstate::calibration: return calibrate(x ,y);
    default:                    return;
    }
}

/**
 * @copydoc app::calibrate
 */
auto app::calibrate(int x, int y) -> void {
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
}

/**
 * @brief Event-based mechanics of this application that are yet to be implemented.
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
