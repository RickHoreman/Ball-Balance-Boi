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

#include "ofxCv.h"

#include <opencv.hpp>
#include <tracking.hpp> // ?



#define MINTRACKAREA 50

using namespace cv;

auto ofApp::trackball(std::uint8_t* frameptr) -> void {

    // cv::Mat my_mat(rows, cols, CV_8UC1, &buf[0]); //in case of BGR image use CV_8UC3
    Mat frame{camcfg.frame.height, camcfg.frame.width, CV_8UC3, frameptr};
    // Mat frame{camcfg.frame.width, camcfg.frame.height, CV_8UC1, frameptr};

    //Resize large images to reduce processing load
    // cap >> frame;



    //Convert RGB to HSV colormap
    //and apply Gaussain blur
    Mat hsvFrame;
    cvtColor(frame, hsvFrame, CV_RGB2HSV);
    // cvtColor(frame, hsvFrame, CV_BGR2HSV);

    blur(hsvFrame, hsvFrame, cv::Size(1, 1));

    //Threshold 
    // Scalar lowerBound = cv::Scalar(55, 100, 50);
    // Scalar upperBound = cv::Scalar(90, 255, 255);
    Scalar lowerBound = cv::Scalar(0, 100, 50);
    Scalar upperBound = cv::Scalar(50, 255, 255);
    Mat threshFrame;
    inRange(hsvFrame, lowerBound, upperBound, threshFrame);

    //Calculate X,Y centroid
    Moments m = moments(threshFrame, false);
    Point com(m.m10 / m.m00, m.m01 / m.m00);

    //Draw crosshair
    Scalar color = cv::Scalar(0, 0, 255);
    drawMarker(frame, com, color, cv::MARKER_CROSS, 50, 5);

    // imshow("Tennis Ball", frame);
    // imshow("Thresholded Tennis Ball", threshFrame);
    // ofxCv::toOf(frame, colorimg);
    ofxCv::drawMat(frame, 0, 0);
    // ofxCv::drawMat(threshFrame, 600, 400);

    string str = "pos: ";
    str += ofToString(com.x, 2);
    str += ", ";
    str += ofToString(com.y, 2);
    ofDrawBitmapString(str, 10, 50);

    std::cout << com.x << ' ' << com.y << std::endl;

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
    camframe = std::make_unique_for_overwrite<std::uint8_t[]>(camcfg.frame.size(3));
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
    trackball(camframe.get());
    
    string str = "app fps: ";
	str += ofToString(ofGetFrameRate(), 2);
    str += "\ncamera fps: " + ofToString(camstats.fps(), 2);
    ofDrawBitmapString(str, 10, 15);

    stringstream reportStr;
    reportStr << "bg subtraction and blob detection" << endl
        << "press ' ' to capture bg" << endl
        << "threshold " << threshold << " (press: +/-)" << endl
        << "num blobs found " << finder.nBlobs << ", fps: " << ofGetFrameRate();
    ofDrawBitmapString(reportStr.str(), 1300, 200);
}

/**
 * @copydoc ofApp::drawblobs
 * @internal ..
 */
auto ofApp::drawblobs() -> void {
    for (int i = 0; i < finder.nBlobs; i++) {
        finder.blobs[i].draw(camcfg.frame.width, camcfg.frame.height);

        // draw over the centroid if the blob is a hole
        ofSetColor(255);
        if (finder.blobs[i].hole) {
            ofDrawBitmapString("hole",
                finder.blobs[i].boundingRect.getCenter().x + 360,
                finder.blobs[i].boundingRect.getCenter().y + 540);
        }
    }
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
auto ofApp::mousePressed(int x, int y, int button) -> void {}
auto ofApp::mouseReleased(int x, int y, int button) -> void {}
auto ofApp::windowResized(int w, int h) -> void {}
auto ofApp::gotMessage(ofMessage msg) -> void {}
auto ofApp::dragEvent(ofDragInfo dragInfo) -> void {}
