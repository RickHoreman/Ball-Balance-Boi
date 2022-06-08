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
// #include "memory.h"

#include <exception>
#include <iostream>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>

#include "ofxCv.h"

#include <opencv.hpp>
#include <tracking.hpp> // ?



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
        200, 20, 20, 75 // change the last two parameters
   // (min_radius & max_radius) to detect larger circles
    );

    for (size_t i = 0; i < circles.size(); i++)
    {
        cv::Vec3i c = circles[i];
        cv::Point center = cv::Point(c[0], c[1]);
        // circle center
        circle(frame, center, 1, cv::Scalar(0, 100, 100), 3, cv::LINE_AA);
        // circle outline
        int radius = c[2];
        circle(frame, center, radius, cv::Scalar(255, 0, 255), 3, cv::LINE_AA);
        //std::cout << i << ": " << center.x << ";" << center.y << "\n";
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
    allocframebuffers();
}

/**
 * @copydoc ofApp::allocframebuffers
 * @internal ..
 */
auto ofApp::allocframebuffers() -> void {
    // grayimg.allocate(camcfg.frame.width, camcfg.frame.height);
    // bgimg.allocate(camcfg.frame.width, camcfg.frame.height);
    // diffimg.allocate(camcfg.frame.width, camcfg.frame.height);
    camframe = std::make_unique_for_overwrite<std::uint8_t[]>(camcfg.frame.size());
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
    // //

    //stringstream reportStr;
    //reportStr << "bg subtraction and blob detection" << endl
    //    << "press ' ' to capture bg" << endl
    //    << "threshold " << threshold << " (press: +/-)" << endl
    //    << "num blobs found " << finder.nBlobs << ", fps: " << ofGetFrameRate();
    //ofDrawBitmapString(reportStr.str(), 1300, 200);
}

auto ofApp::drawDebug() -> void {
    for (int i{ 0 }; i < debugLines.size(); i++) {
        ofSetColor(debugLineColors[i]);
        debugLines[i].draw();
    }

    if (state == appState::running) {
        for (int i{ 0 }; i < transMatrices.size(); i++) {
            ofPoint mousePos{ float(ofGetMouseX()), float(ofGetMouseY()) };

            float result = (mousePos.x - centerPoint.x) * transMatrices[i].x + (mousePos.y - centerPoint.y) * transMatrices[i].y; // This should happen elsewhere (too) probably idunno

            ofPoint v = calibrationPoints[i] - centerPoint;
            float mV = std::sqrt(std::pow(v.x, 2) + std::pow(v.y, 2));
            ofPoint resPos = (v / mV) * result;

            ofPolyline resLine;
            resLine.addVertices({ mousePos, resPos + centerPoint });
            ofColor color;
            switch (i) {
            case 0: color = { 255,255,0 }; break;
            case 1: color = { 0,255,255 }; break;
            case 2: color = { 255,0,255 }; break;
            }
            ofSetColor(color);
            resLine.draw();

            //Scaled output shown as sliders:
            float targetSideSize = 250; // Move target related calculations stuff elsewhere (only needs to happen once)
            float targetHeight = std::sqrt(std::pow(targetSideSize, 2) - std::pow(targetSideSize / 2.f, 2));
            std::array<ofPoint, 3> targetPoints{ ofPoint{targetSideSize / 2.f, 0.f}, ofPoint{0.f, targetHeight}, ofPoint{targetSideSize, targetHeight} };
            ofPoint targetCenter{(targetPoints[0].x + targetPoints[1].x + targetPoints[2].x)/3.f, (targetPoints[0].y + targetPoints[1].y + targetPoints[2].y) / 3.f };
            float targetScale = targetCenter.x;
            float scaledRes = result / mV * targetScale;
            
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
        }
    }

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
    transMatrices[i] = vTrans;
};

auto ofApp::finishCalibration() -> void {
    std::cout << "Finishing calibration.\n";

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

    genTransMatrix(0);
    genTransMatrix(1);
    genTransMatrix(2);

    state = appState::running;
    std::cout << "Finished calibration.\n";
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
    auto const update_threshold{
        [&](int inc, int limit)
        { threshold += threshold == limit ? 0 : inc; }};
    switch (key) {
    break; case ' ': setbackground();
    break; case '+': update_threshold(1, 255);
    break; case '-': update_threshold(-1, 0);
    break; case 'c': reCalibrate();
    }
}

auto ofApp::mousePressed(int x, int y, int button) -> void {
    switch (button) {
    case 0:
        switch (state) {
        case appState::calibration:
            calibrationPoints[pointsCalibrated] = { float(x), float(y) };
            pointsCalibrated++;
            std::cout << "Point: " << pointsCalibrated << " x: " << x << " y: " << y << "\n";
            ofPolyline line;
            line.addVertex(ofPoint{ 640 / 2, 480 / 2 });
            line.addVertex(ofPoint{ float(x), float(y) });
            debugLines.push_back(line);
            debugLineColors.push_back({ 100, 0, 0 });
            if(pointsCalibrated >= 3){
                finishCalibration();
            }
            break;
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

/**
 * @copydoc ofApp::setbackground
 * @internal ..
 */
auto ofApp::setbackground() -> void
{ bgimg = grayimg; }

auto ofApp::keyReleased(int key) -> void{}
auto ofApp::mouseMoved(int x, int y ) -> void {}
auto ofApp::mouseDragged(int x, int y, int button) -> void {}
//auto ofApp::mousePressed(int x, int y, int button) -> void {}
auto ofApp::mouseReleased(int x, int y, int button) -> void {}
auto ofApp::windowResized(int w, int h) -> void {}
auto ofApp::gotMessage(ofMessage msg) -> void {}
auto ofApp::dragEvent(ofDragInfo dragInfo) -> void {}
