/**
 * @file       application.h
 * @version    0.1
 * @date       May 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief ..
 * @details ..
 */

#ifndef OF_APPLICATION_H
#define OF_APPLICATION_H

#include "ps3eye.h"
#include "camera.h"
#include "config.h"
#include "memory.h"
#include "menu.h"
#include "serial.h"
#include "types.h"

#include <ofMain.h>
#include <ofBaseApp.h>
#include <ofxOpenCv.h>
#include <opencv.hpp>

#include <functional>
#include <memory>
#include <string>

/**
 * @namespace ..
 * @brief ..
 */
namespace of {

/**
 * @class app
 * @brief ..
 * @details ..
 */
class app : public ofBaseApp {
public:
	/**
	 * @brief ..
	 */
	app() = default;

	/**
	 * @brief ..
	 */
	explicit app(comm::serial& serialcomm)
		: serial{&serialcomm} {}

	/**
	 * @brief ..
	 * @details ..
	 * @{
	 */
	auto setup() -> void override;
	auto exit() -> void override;
	auto update() -> void override;
	auto draw() -> void override;
	/** @} */

	/**
	 * @brief ..
	 * @details ..
	 * @{
	 */
	auto keyPressed(int key) -> void override;
	auto keyReleased(int key) -> void override;
	auto mouseMoved(int x, int y ) -> void override;
	auto mouseDragged(int x, int y, int button) -> void override;
	auto mousePressed(int x, int y, int button) -> void override;
	auto mouseReleased(int x, int y, int button) -> void override;
	auto windowResized(int w, int h) -> void override;
	auto dragEvent(ofDragInfo dragInfo) -> void override;
	auto gotMessage(ofMessage msg) -> void override;
	/** @} */

private:
	/**
	 * @brief ..
	 */
	auto trackball() -> void;

	/**
	 * @brief ..
	 * @details ..
	 * @param[in] .. ..
	 * @param[in] .. ..
	 * @{
	 */
	auto drawcamera(float x, float y) const -> void;
	auto drawmenu(float x, float y) const -> void;
	auto drawfps(float x, float y) const -> void;
	/** @} */

	/**
	 * @brief ..
	 * @details ..
	 * @{
	 */
	auto initmenu() noexcept -> void;
	auto showmenu() noexcept -> void;
	auto exitmenu() noexcept -> void;
	/** @} */

	/**
	 * @brief ..
	 * @details ..
	 * @param[in] .. ..
	 * @{
	 */
	auto handle_appinput(int key) noexcept -> void;
	auto handle_menuinput(int key) noexcept -> void;
	auto handle_inputvalue(int key) -> void;
	/** @} */

	/**
	 * @brief ..
	 * @details ..
	 * @param[in] .. ..
	 */
	auto select_option(int key) noexcept -> void;
	
	/**
	 * @brief ..
	 * @details ..
	 * @param[in] .. ..
	 * @{
	 */
	auto apply_inputvalue() -> void;
	auto erase_inputvalue() noexcept -> void;
	auto add_inputvalue(unsigned char key) noexcept -> void;
	/** @} */

	using action_type = std::function<void(cfg::cfgitem<> const&)>; /**< .. */
	using menu_type = opt::menu<cfg::cfgitem<>, action_type>;       /**< .. */

	/**
	 * @enum ..
	 * @brief ..
	 */
	enum class keyinput {
		app,  /**< .. */
		menu, /**< .. */
		value /**< .. */
	};

	cfg::config appcfg{cfg::config::defaults()}; /**< .. */
	mem::access_ptr<comm::serial> serial;        /**< .. */

    cam::devptr camera;                /**< .. */
	cam::framestats camstats;		   /**< .. */
	std::unique_ptr<uint8[]> camframe; /**< .. */
	cv::Mat trackframe;				   /**< .. */
	cv::Point ballpos;				   /**< .. */

	keyinput inputmode{};    /**< .. */
	menu_type cfgmenu;		 /**< .. */
	std::string inputvalue;  /**< .. */
	std::string valueprompt; /**< .. */
	std::string menuprompt;  /**< .. */
};

} // namespace of

#endif