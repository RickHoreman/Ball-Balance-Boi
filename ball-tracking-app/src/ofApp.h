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

#ifndef BBB_CAM_APP_H
#define BBB_CAM_APP_H

#include "ps3eye.h"
#include "camera.h"

#include <ofMain.h>
#include <ofBaseApp.h>
#include <ofxOpenCv.h>

#include <memory>
#include <array>
#include <vector>

#include <opencv.hpp>

enum appState {
	running,
	calibration
};

/**
 * @class ofApp
 * @brief ..
 * @details ..
 */
class ofApp : public ofBaseApp {
public:
	/**
	 * @brief ..
	 */
	ofApp() = default;

	auto setup() -> void override;
	auto exit() -> void override;
	auto update() -> void override;
	auto draw() -> void override;

	auto drawDebug() -> void;

	auto genTransMatrix(int i) -> void;
	auto finishCalibration() -> void;
	auto setSetPoint(int x, int y) -> void;
	auto reCalibrate() -> void;

	auto keyPressed(int key) -> void override;
	auto keyReleased(int key) -> void override;
	auto mouseMoved(int x, int y ) -> void override;
	auto mouseDragged(int x, int y, int button) -> void override;
	auto mousePressed(int x, int y, int button) -> void override;
	auto mouseReleased(int x, int y, int button) -> void override;
	auto windowResized(int w, int h) -> void override;
	auto dragEvent(ofDragInfo dragInfo) -> void override;
	auto gotMessage(ofMessage msg) -> void override;

private:
	auto allocframebuffers() -> void;
	auto setbackground() -> void;
	//auto drawblobs() -> void;
	auto trackball() -> void;

    cam::devptr camera;
	cam::framestats camstats;
	cam::config camcfg{cam::config::defaults()};
	std::unique_ptr<std::uint8_t[]> camframe;
	cv::Mat frame{ camcfg.frame.height, camcfg.frame.width, CV_8UC1 };

	ofxCvColorImage colorimg;
	//
	ofxCvGrayscaleImage grayimg;
	ofxCvGrayscaleImage bgimg{};
	ofxCvGrayscaleImage diffimg;
	ofxCvContourFinder finder;

	int threshold{80};

	bool displayDebugVisualisation = true;

	appState state = calibration;
	int pointsCalibrated = 0;
	std::array<ofPoint, 3> calibrationPoints;
	std::array<ofPoint, 3> transMatricesPreScale;
	std::array<ofPoint, 3> transMatrices;

	ofPoint targetCenter;
	float targetScale;

	ofPoint centerPoint;

	ofPoint ballPos;
	ofPoint setPoint;
	std::array<float, 3> ballPosPerAxis;
	std::array<float, 3> setPointPerAxis;

	std::vector<ofPolyline> debugLines;
	std::vector<ofColor> debugLineColors;
};

#endif
