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
// #include "memory.h"

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



#define MINTRACKAREA 50

auto ofApp::trackball() -> void {

    // cv::Mat my_mat(rows, cols, CV_8UC1, &buf[0]); //in case of BGR image use CV_8UC3
    
    // Mat frame{camcfg.frame.width, camcfg.frame.height, CV_8UC1, frameptr};

    //Resize large images to reduce processing load
    // cap >> frame;



    //Convert RGB to HSV colormap
    //and apply Gaussain blur
    //Mat hsvFrame;
    //cvtColor(frame, hsvFrame, CV_RGB2HSV);
    // cvtColor(frame, hsvFrame, CV_BGR2HSV);

    //blur(hsvFrame, hsvFrame, cv::Size(1, 1));

    //Threshold 
    // Scalar lowerBound = cv::Scalar(55, 100, 50);
    // Scalar upperBound = cv::Scalar(90, 255, 255);
    //Scalar lowerBound = cv::Scalar(0, 100, 50);
    //Scalar upperBound = cv::Scalar(50, 255, 255);
    //Mat threshFrame;
    //inRange(hsvFrame, lowerBound, upperBound, threshFrame);

    //Calculate X,Y centroid

    //Moments m = moments(threshFrame, false);
    //Point com(m.m10 / m.m00, m.m01 / m.m00);

    //Hough method
    //cv::Mat gray = frame;
    //cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    //medianBlur(frame, frame, 5);

    //cv::equalizeHist(frame, frame);
    //cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);

    vector<cv::Vec3f> circles;
    HoughCircles(frame, circles, cv::HOUGH_GRADIENT, 1,
        frame.rows / 16,  // change this value to detect circles with different distances to each other
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
            ballPos = { float(center.x), float(center.y) };
            string output;
            for (int j{}; j < 3; j++) {
                ballPosPerAxis[j] = (ballPos.x - centerPoint.x) * transMatrices[j].x + (ballPos.y - centerPoint.y) * transMatrices[j].y;
                output += std::format("{:.2f} {:.2f} ", ballPosPerAxis[j], setPointPerAxis[j]);
            }
            std::cout << output << std::endl;
        }
    }

    //Draw crosshair
    //Scalar color = cv::Scalar(0, 0, 255);
    //drawMarker(frame, com, color, cv::MARKER_CROSS, 50, 5);

    // imshow("Tennis Ball", frame);
    // imshow("Thresholded Tennis Ball", threshFrame);
    // ofxCv::toOf(frame, colorimg);
    // ofxCv::drawMat(threshFrame, 600, 400);

    //string str = "pos: ";
    //str += ofToString(com.x, 2);
    //str += ", ";
    //str += ofToString(com.y, 2);
    //ofDrawBitmapString(str, 10, 50);

    //std::cout << com.x << ' ' << com.y << std::endl;

    // return threshFrame;
};

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
    auto const framesize = camcfg.frame.size(camera->getOutputBytesPerPixel());
    camframe = std::make_unique_for_overwrite<std::uint8_t[]>(framesize);
    frame = cv::Mat{camcfg.frame.height, camcfg.frame.width,
        CV_8UC1, camframe.get()};
    ofSetFrameRate(75);
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

//char key
//camcfg& 
//
//b - brightness: 678
//c - contrast:   456
//s - sharpness:  789
//
//new value: 

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
    frame.data = camframe.get();

    trackball();

    //genTransMatrix(0, { 255,255,0 });
    //genTransMatrix(1, { 0,255,255 });
    //genTransMatrix(2, { 255,0,255 });

    // colorimg.setFromPixels(camframe.get(), camcfg.frame.width, camcfg.frame.height);
    // trackball(camframe.get());

    // grayimg.setFromPixels(camframe.get(), camcfg.frame.width, camcfg.frame.height);
    // grayimg = colorimg;
    //diffimg.absDiff(bgimg, grayimg);
    //diffimg.threshold(threshold);
    //finder.findContours(diffimg, 20, camcfg.frame.size() / 3, 10, true);
    //camstats.update();
}

/**
 * @copydoc ofApp::draw
 * @internal ..
 */
auto ofApp::draw() -> void {
    ofSetHexColor(0xffffff);

    // grayimg.draw(0, 0);
    // bgimg.draw(0, camcfg.frame.height);
    // diffimg.draw(camcfg.frame.width, 0);

    // drawblobs();

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
    // //

    //stringstream reportStr;
    //reportStr << "bg subtraction and blob detection" << endl
    //    << "press ' ' to capture bg" << endl
    //    << "threshold " << threshold << " (press: +/-)" << endl
    //    << "num blobs found " << finder.nBlobs << ", fps: " << ofGetFrameRate();
    //ofDrawBitmapString(reportStr.str(), 1300, 200);
}

/**
 * @copydoc ofApp::drawmenu
 * @internal ..
 */
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

            float result = (ballPos.x - centerPoint.x) * transMatricesPreScale[i].x + (ballPos.y - centerPoint.y) * transMatricesPreScale[i].y; // This should happen elsewhere (too) probably idunno

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
    //float scale = 200;
    ofPoint vTrans{ 0, 1 };
    ofPoint center = centerPoint;
    ofPoint v = calibrationPoints[i] - center;

    float mV = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
    //scale = mV;
    vTrans = vTrans /*/ mPre*//* * (scale / mPre)*/;
    float rTrans = std::atan((v.y) / (v.x)) - std::atan(vTrans.y / vTrans.x);
    if (v.x < 0 or (v.x == 0 and v.y < 0)) {
        rTrans += M_PI;
    }
    vTrans = { std::cos(rTrans) * vTrans.x - std::sin(rTrans) * vTrans.y, std::sin(rTrans) * vTrans.x + std::cos(rTrans) * vTrans.y };
    transMatricesPreScale[i] = vTrans;

    transMatrices[i] = vTrans / mV * targetScale;
};

auto ofApp::finishCalibration() -> void {
    //std::cout << "Finishing calibration.\n";

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

    float targetSideSize = 250; // Move target related calculations stuff elsewhere (only needs to happen once)
    float targetHeight = std::sqrt(std::pow(targetSideSize, 2) - std::pow(targetSideSize / 2.f, 2));
    std::array<ofPoint, 3> targetPoints{ ofPoint{targetSideSize / 2.f, 0.f}, ofPoint{0.f, targetHeight}, ofPoint{targetSideSize, targetHeight} };
    targetCenter = { (targetPoints[0].x + targetPoints[1].x + targetPoints[2].x) / 3.f, (targetPoints[0].y + targetPoints[1].y + targetPoints[2].y) / 3.f };
    targetScale = targetCenter.x;

    genTransMatrix(0);
    genTransMatrix(1);
    genTransMatrix(2);

    setSetPoint(640/2, 480/2);

    state = appState::running;
    //std::cout << "Finished calibration.\n";
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
    break; case 'x': reCalibrate();
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

    auto result = std::uint8_t{ 255 };
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

auto ofApp::mousePressed(int x, int y, int button) -> void {
    switch (button) {
    case 0:
        switch (state) {
        case appState::calibration: {
            calibrationPoints[pointsCalibrated] = { float(x), float(y) };
            pointsCalibrated++;
            //std::cout << "Point: " << pointsCalibrated << " x: " << x << " y: " << y << "\n";
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
            //std::cout << "new setPoint x: " << x << " y: " << y << "\n";
            break;
        }
        }
        break;
    case 1:
        //std::cout << "x: " << x << " y: " << y << " center\n";
        break;
    case 2:
        //std::cout << "x: " << x << " y: " << y << " right\n";
        break;
    }
}

auto ofApp::keyReleased(int key) -> void{}
auto ofApp::mouseMoved(int x, int y ) -> void {}
auto ofApp::mouseDragged(int x, int y, int button) -> void {}
//auto ofApp::mousePressed(int x, int y, int button) -> void {}
auto ofApp::mouseReleased(int x, int y, int button) -> void {}
auto ofApp::windowResized(int w, int h) -> void {}
auto ofApp::gotMessage(ofMessage msg) -> void {}
auto ofApp::dragEvent(ofDragInfo dragInfo) -> void {}
