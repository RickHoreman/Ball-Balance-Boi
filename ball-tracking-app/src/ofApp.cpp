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

// #define OF_USING_STD_FS 1
#include "ofApp.h"
#include "ps3eye.h"
#include "camera.h"
#include "menu.h"
#include "serial.h"

#include "ofxCv.h"
#include <opencv.hpp>
#include <tracking.hpp> // ?

#define _USE_MATH_DEFINES
#include <math.h>
#include <cctype>
#include <charconv>
#include <exception>
#include <format>
#include <iostream>
#include <memory>
#include <algorithm>


#define MINTRACKAREA 50

auto ofApp::trackball() -> void {

    vector<cv::Vec3f> circles;
    HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1,
        1000,  // change this value to detect circles with different distances to each other
        200, 20, 10, 75 // change the last two parameters
   // (min_radius & max_radius) to detect larger circles
    );

    for (size_t i = 0; i < circles.size(); i++)
    {
        if (i == 0) {
            cv::Vec3i c = circles[i];
            cv::Point center = cv::Point(c[0], c[1]);
            // circle center
            circle(frame, center, 1, cv::Scalar(0, 100, 100), 3, cv::LINE_AA);
            // circle outline
            int radius = c[2];
            circle(frame, center, radius, cv::Scalar(255, 0, 255), 3, cv::LINE_AA);
            //std::cout << i << ": " << center.x << ";" << center.y << "\n";
            if (state == appState::running) {
                ballPos = { float(center.x), float(center.y) };
                string output;
                for (int j{}; j < 3; j++) {
                    ballPosPerAxis[j] = (ballPos.x - centerPoint.x) * transMatrices[j].x + (ballPos.y - centerPoint.y) * transMatrices[j].y;
                    output += std::format("{:.5f} {:.5f} ", ballPosPerAxis[j], setPointPerAxis[j]);
                }
                //std::cout << output << std::endl;
                pid();
            }
        }
    }
}

auto ofApp::pid() -> void {
    string output;
    string dbg;
    for (int i{}; i < 3; i++) {
        double error = setPointPerAxis[i] - ballPosPerAxis[i];
        iError[i] += error;
        servoAction[i] = kp * error + ki * iError[i] + kd * (error - prevError[i]);
        servoAction[i] = std::max(std::min(servoAction[i], 45.0), -10.0);
        prevError[i] = error;
        output += std::format("{:.5f} ", servoAction[i] + 45.0);
        dbg += std::format("{:.5f} {:.5f} {:.5f} ; ", kp * error, ki * iError[i], kd * (error - prevError[i]));
    }
    output += "\n";
    std::cout << output;
    //std::cout << dbg << std::endl;
    serialcomm->write(output);
}

auto ofApp::setup() -> void {
    if (camera = cam::getdevice(); not camera) {
        throw std::runtime_error("could not find ps3 camera");
    }
    if (cam::initcamera(*camera, camcfg) != cam::status::operational) {
        throw std::runtime_error("could not initialize ps3 camera");
    }
    auto const framesize = camcfg.frame.size(camera->getOutputBytesPerPixel());
    camframe = std::make_unique_for_overwrite<std::uint8_t[]>(framesize);
    frame = cv::Mat{camcfg.frame.height, camcfg.frame.width,
        CV_8UC1, camframe.get()};
    ofSetFrameRate(75);
    serialcomm->setup();
    initmenu();
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

auto ofApp::exit() -> void {
    if (camera) {
        camera->stop();
    }
}

auto ofApp::update() -> void {
    if (not camera) return;

	camera->getFrame(camframe.get());
    frame.data = camframe.get();

    trackball();
}

auto ofApp::draw() -> void {
    ofSetHexColor(0xffffff);

    ofxCv::drawMat(frame, 0, 0, GL_R8);


    if (displayDebugVisualisation) { drawDebug(); }


    string str = "app fps: ";
	str += ofToString(ofGetFrameRate(), 2);
    str += "\ncamera fps: " + ofToString(camstats.fps(), 2);
    ofDrawBitmapString(str, 10, 15);

    switch (state) {
    case appState::calibration:
        string str = "Please click calibration point ";
        str += ofToString(pointsCalibrated + 1);
        str += ".\n";
        ofDrawBitmapString(str, 190, 200);
    }

    drawmenu(10, 150);
}

auto ofApp::drawmenu(float x, float y) const -> void {
    ofDrawBitmapString(std::format("{}\n{}{}",
        menuprompt, inputprompt, inputvalue), x, y);
}

auto ofApp::drawDebug() -> void {
    for (int i{ 0 }; i < debugLines.size(); i++) {
        ofSetColor(debugLineColors[i]);
        debugLines[i].draw();
    }

    if (state == appState::running) {
        for (int i{ 0 }; i < transMatrices.size(); i++) {

            float result = (ballPos.x - centerPoint.x) * transMatricesPreScale[i].x + (ballPos.y - centerPoint.y) * transMatricesPreScale[i].y;

            ofPoint v = calibrationPoints[i] - centerPoint;
            float mV = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
            ofPoint resPos = (v / mV) * result;

            ofPolyline resLine;
            resLine.addVertices({ ballPos, resPos + centerPoint });
            ofColor color;
            switch (i) {
            case 0: color = { 255,255,0 }; break;
            case 1: color = { 0,255,255 }; break;
            case 2: color = { 255,0,255 }; break;
            }
            ofSetColor(color);
            resLine.draw();

            //Scaled output shown as sliders:
            
            float scaledRes = (ballPos.x - centerPoint.x) * transMatrices[i].x + (ballPos.y - centerPoint.y) * transMatrices[i].y;
            
            ofPoint displayPos{ 650.f, 50.f + 30.f*i };

            ofPolyline scale;
            scale.addVertices({ displayPos, displayPos + ofPoint{targetScale * 2.f, 0} });
            ofPolyline pointer;
            pointer.addVertices({ displayPos + ofPoint{scaledRes + targetScale, -5}, displayPos + ofPoint{scaledRes + targetScale, 5} });
            ofPolyline center;
            center.addVertices({ displayPos + ofPoint{targetScale, -3}, displayPos + ofPoint{targetScale, 3} });
            scale.draw();
            pointer.draw();
            center.draw();

            string str = std::format("{:.2f}", scaledRes );
            ofDrawBitmapString(str, displayPos + ofPoint{ targetScale*2.f + 10, 0});

            color.a = 128;
            ofSetColor(color);
            ofPolyline helper;
            helper.addVertices({ resPos + centerPoint, centerPoint });
            helper.draw();

            ofSetColor({ 255, 128, 128 });
            ofPolyline setpointer;
            setpointer.addVertices({ displayPos + ofPoint{setPointPerAxis[i] + targetScale, -5}, displayPos + ofPoint{setPointPerAxis[i] + targetScale, 5}});
            setpointer.draw();
        }
    }

    ofSetColor({ 255, 128, 128 });
    ofDrawCircle(setPoint, 5.f);

    ofSetColor({ 255,255,255 });
}

auto ofApp::genTransMatrix(int i) -> void {
    ofPoint vTrans{ 0, 1 };
    ofPoint center = centerPoint;
    ofPoint v = calibrationPoints[i] - center;

    float mV = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
    float rTrans = std::atan((v.y) / (v.x)) - std::atan(vTrans.y / vTrans.x);
    if (v.x < 0 or (v.x == 0 and v.y < 0)) {
        rTrans += M_PI;
    }
    vTrans = { std::cos(rTrans) * vTrans.x - std::sin(rTrans) * vTrans.y, std::sin(rTrans) * vTrans.x + std::cos(rTrans) * vTrans.y };
    transMatricesPreScale[i] = vTrans;

    transMatrices[i] = vTrans / mV * targetScale;
};

auto ofApp::finishCalibration() -> void {
    centerPoint = { (calibrationPoints[0].x + calibrationPoints[1].x + calibrationPoints[2].x) / 3, (calibrationPoints[0].y + calibrationPoints[1].y + calibrationPoints[2].y) / 3 };

    for (int i{ 0 }; i < calibrationPoints.size(); i++) {
        int j = i + 1;
        if (j >= calibrationPoints.size()) {
            j = 0;
        }
        ofPolyline line;
        line.addVertex(centerPoint);
        line.addVertex(calibrationPoints[i]);
        debugLines.push_back(line);
        debugLineColors.push_back({ 0,0,150 });

        ofPolyline line2;
        line2.addVertex(calibrationPoints[i]);
        line2.addVertex(calibrationPoints[j]);
        debugLines.push_back(line2);
        debugLineColors.push_back({ 0,200,0 });
    }

    debugLines.erase(debugLines.begin(), debugLines.begin() + 3);
    debugLineColors.erase(debugLineColors.begin(), debugLineColors.begin() + 3);

    float targetSideSize = 250;
    float targetHeight = std::sqrt(std::pow(targetSideSize, 2) - std::pow(targetSideSize / 2.f, 2));
    std::array<ofPoint, 3> targetPoints{ ofPoint{targetSideSize / 2.f, 0.f}, ofPoint{0.f, targetHeight}, ofPoint{targetSideSize, targetHeight} };
    targetCenter = { (targetPoints[0].x + targetPoints[1].x + targetPoints[2].x) / 3.f, (targetPoints[0].y + targetPoints[1].y + targetPoints[2].y) / 3.f };
    targetScale = targetCenter.x;

    genTransMatrix(0);
    genTransMatrix(1);
    genTransMatrix(2);

    setSetPoint(640/2, 480/2);

    state = appState::running;
}

auto ofApp::setSetPoint(int x, int y) -> void {
    setPoint = { float(x), float(y) };
    for (int i{}; i < 3; i++) {
        setPointPerAxis[i] = (x - centerPoint.x) * transMatrices[i].x + (y - centerPoint.y) * transMatrices[i].y;
    }
}

auto ofApp::reCalibrate() -> void {
    state = appState::calibration;
    debugLines.clear();
    debugLineColors.clear();
    pointsCalibrated = 0;
}

auto ofApp::keyPressed(int key) -> void {
    switch (inputmode) {
        case keyinput::app:   return handle_app_input(key);
        case keyinput::menu:  return handle_menu_input(key);
        case keyinput::value: return handle_input_value(key);
        default:              return;
    }
}

auto ofApp::handle_app_input(int key) noexcept -> void {
    switch (key) {
    case OF_KEY_TAB: return show_menu();
    break; case 'x': reCalibrate();
    default:         return;
    }
}

auto ofApp::show_menu() noexcept -> void {
    menuprompt = inputmenu.to_string(*camera);
    inputmode = keyinput::menu;
}

auto ofApp::handle_menu_input(int key) noexcept -> void {
    switch (key) {
    case OF_KEY_TAB: return exit_menu();
    default:         return select_option(key);
    }
}

auto ofApp::exit_menu() noexcept -> void {
    menuprompt.clear();
    inputmode = keyinput::app;
}

auto ofApp::select_option(int key) noexcept -> void {
    if (not inputmenu.contains(key)) return;

    inputmenu.select(key);
    inputprompt = "new value: ";
    inputmode = keyinput::value;
}

auto ofApp::handle_input_value(int key) -> void {
    switch (key) {
    case OF_KEY_RETURN:    return apply_input_value();
    case OF_KEY_BACKSPACE: return erase_input_value();
    default:               return add_input_value(key);
    }
}

auto ofApp::apply_input_value() -> void {
    if (inputvalue.empty()) return;

    auto result = std::uint8_t{ 255 };
    auto const status = std::from_chars(inputvalue.data(),
        inputvalue.data() + inputvalue.size(), result);
    inputmenu.selection().setvalue(*camera, result);
    menuprompt = inputmenu.to_string(*camera);
    inputprompt.clear();
    inputvalue.clear();
    inputmode = keyinput::menu;
}

auto ofApp::erase_input_value() noexcept -> void {
    if (inputvalue.empty()) return;
    inputvalue.pop_back();
}

auto ofApp::add_input_value(unsigned char key) noexcept -> void {
    if (not std::isdigit(key)) return;
    inputvalue += key;
}

auto ofApp::mousePressed(int x, int y, int button) -> void {
    switch (button) {
    case 0:
        switch (state) {
        case appState::calibration: {
            calibrationPoints[pointsCalibrated] = { float(x), float(y) };
            pointsCalibrated++;
            ofPolyline line;
            line.addVertex(ofPoint{ 640 / 2.f, 480 / 2.f });
            line.addVertex(ofPoint{ float(x), float(y) });
            debugLines.push_back(line);
            debugLineColors.push_back({ 100, 0, 0 });
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

auto ofApp::keyReleased(int key) -> void{}
auto ofApp::mouseMoved(int x, int y ) -> void {}
auto ofApp::mouseDragged(int x, int y, int button) -> void {}
auto ofApp::mouseReleased(int x, int y, int button) -> void {}
auto ofApp::windowResized(int w, int h) -> void {}
auto ofApp::gotMessage(ofMessage msg) -> void {}
auto ofApp::dragEvent(ofDragInfo dragInfo) -> void {}
