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
#include "menu.h"
#include "serial.h"

#include <ofMain.h>
#include <ofBaseApp.h>
#include <ofxOpenCv.h>
#include <opencv.hpp>

#include <memory>
#include <array>
#include <vector>
#include <cstdint>
#include <string>
#include <tuple>

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
	ofApp(comm::serial& serial)
		: serialcomm{ &serial } {}

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
	auto handle_app_input(int key) noexcept -> void;
	auto handle_menu_input(int key) noexcept -> void;
	auto handle_input_value(int key) -> void;
	auto show_menu() noexcept -> void;
	auto exit_menu() noexcept -> void;
	auto select_option(int key) noexcept -> void;
	auto apply_input_value() -> void;
	auto erase_input_value() noexcept -> void;
	auto add_input_value(unsigned char key) noexcept -> void;
	auto initmenu() noexcept -> void;
	auto drawcamera(float x, float y) const -> void;
	auto drawmenu(float x, float y) const -> void;
	auto drawfps(float x, float y) const -> void;
	auto trackball() -> void;
	auto pid() -> void;

    cam::devptr camera;
	cam::framestats camstats;
	cam::config camcfg{cam::config::defaults()};
	std::unique_ptr<std::uint8_t[]> camframe;
	cv::Mat frame;

	ofxCvColorImage colorimg;
	//
	ofxCvGrayscaleImage grayimg;
	ofxCvGrayscaleImage bgimg{};
	ofxCvGrayscaleImage diffimg;
	ofxCvContourFinder finder;

	enum class keyinput { app, menu, value };
	keyinput inputmode{};
	using getter_t = std::uint8_t(*)(cam::ps3cam const&);
	using setter_t = void (*)(cam::ps3cam&, std::uint8_t);
	opt::menu<getter_t, setter_t> inputmenu;
	std::string inputvalue;
	std::string inputprompt;
	std::string menuprompt;

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

	double kp=0.5;
	double ki=0.000001;
	double kd = 7.f;
	std::array<double, 3> prevError{0.f,0.f,0.f};
	std::array<double, 3> iError{0.f,0.f,0.f};
	std::array<double, 3> servoAction{ 0.f,0.f,0.f };

	template<typename T>
	using access_ptr = T*;
	access_ptr<comm::serial> serialcomm;
};

#endif
