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

#include <cstdint>
#include <memory>
#include <string>
#include <tuple>

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

	/**
	 * @brief ..
	 */
	ofApp(comm::serial& serial)
		: serialcomm{&serial} {}

	auto setup() -> void override;
	auto exit() -> void override;
	auto update() -> void override;
	auto draw() -> void override;

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

    cam::devptr camera;
	cam::framestats camstats;
	cam::config camcfg{cam::config::defaults()};
	std::unique_ptr<std::uint8_t[]> camframe;
	cv::Mat trackframe;
	cv::Point ballpos;
	template<typename T>
	using access_ptr = T*;
	access_ptr<comm::serial> serialcomm;

	enum class keyinput{app, menu, value};
	keyinput inputmode{};
	using getter_type = std::uint8_t (*)(cam::ps3cam const&);
	using setter_type = void (*)(cam::ps3cam&, std::uint8_t);
	opt::menu<getter_type, setter_type> inputmenu;
	std::string inputvalue;
	std::string inputprompt;
	std::string menuprompt;
};

#endif
