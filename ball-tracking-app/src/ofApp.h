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

#include <ofMain.h>
#include <ofBaseApp.h>
#include <ofxOpenCv.h>
#include <opencv.hpp>

#include <cstdint>
#include <memory>
#include <string>

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
	auto drawmenu(float x, float y) const -> void;
	auto drawfps(float x, float y) const -> void;
	auto trackball() -> void;

	// struct dummycam {
	// 	auto getSharpness() const -> std::uint8_t { return sharpness; }
	// 	auto setSharpness(std::uint8_t val) -> void { sharpness = val; }
	// 	auto getExposure() const -> std::uint8_t { return exposure; }
	// 	auto setExposure(std::uint8_t val) -> void { exposure = val; }
	// 	auto getBrightness() const -> std::uint8_t { return brightness; }
	// 	auto setBrightness(std::uint8_t val) -> void { brightness = val; }
	// 	auto getContrast() const -> std::uint8_t { return contrast; }
	// 	auto setContrast(std::uint8_t val) -> void { contrast = val; }
	// 
	// 	auto getGain() const -> std::uint8_t { return gain; }
	// 	auto setGain(std::uint8_t val) -> void { gain = val; }
	// 	auto getHue() const -> std::uint8_t { return hue; }
	// 	auto setHue(std::uint8_t val) -> void { hue = val; }
	// 
	// 	auto getRedBalance() const -> std::uint8_t { return redbalance; }
	// 	auto setRedBalance(std::uint8_t val) -> void { redbalance = val; }
	// 	auto getGreenBalance() const -> std::uint8_t { return greenbalance; }
	// 	auto setGreenBalance(std::uint8_t val) -> void { greenbalance = val; }
	// 	auto getBlueBalance() const -> std::uint8_t { return bluebalance; }
	// 	auto setBlueBalance(std::uint8_t val) -> void { bluebalance = val; }
	// 
	// 	auto getAutoWhiteBalance() const -> bool { return autowhitebalance; }
	// 	auto setAutoWhiteBalance(bool val) -> void { autowhitebalance = val; }
	// 	auto getAutoGain() const -> bool { return autogain; }
	// 	auto setAutoGain(bool val) -> void { autogain = val; }
	// 
	// 	std::uint8_t sharpness;
	// 	std::uint8_t exposure;
	// 	std::uint8_t brightness;
	// 	std::uint8_t contrast;
	// 
	// 	std::uint8_t gain;
	// 	std::uint8_t hue;
	// 	std::uint8_t redbalance;
	// 	std::uint8_t greenbalance;
	// 	std::uint8_t bluebalance;
	// 
	// 	bool autowhitebalance;
	// 	bool autogain;
	// };
	// std::shared_ptr<dummycam> camera = std::make_shared<dummycam>();

    cam::devptr camera;
	cam::framestats camstats;
	cam::config camcfg{cam::config::defaults()};
	std::unique_ptr<std::uint8_t[]> camframe;

	enum class keyinput{app, menu, value};
	keyinput inputmode{};
	using getter_t = std::uint8_t (*)(cam::ps3cam const&);
	using setter_t = void (*)(cam::ps3cam&, std::uint8_t);
	// using getter_t = std::uint8_t (*)(dummycam const&);
	// using setter_t = void (*)(dummycam&, std::uint8_t);
	opt::menu<getter_t, setter_t> inputmenu;
	std::string inputvalue;
	std::string inputprompt;
	std::string menuprompt;
};

#endif
