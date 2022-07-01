/**
 * @file       application.h
 * @version    0.1
 * @date       May 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief User interface application based upon the OpenFrameworks library.
 */

#ifndef OF_APPLICATION_H
#define OF_APPLICATION_H

#include "camera.h"
#include "config.h"
#include "menu.h"
#include "types.h"
#include "utility.h"

#include <ofMain.h>
#include <ofBaseApp.h>
#include <ofxOpenCv.h>
#include <opencv.hpp>

#include <array>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

/**
 * @namespace of
 * @brief OpenFrameworks related components.
 */
namespace of {

/**
 * @struct serial_error
 * @brief Exception related to serial connections.
 */
struct serial_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * @class app
 * @brief OpenFrameworks user inteface application.
 */
class app : public ofBaseApp {
public:
    /**
     * @brief Default constructs a user interface application.
     */
    app() = default;

    /**
     * @brief Constructs a user interface application with a given configuration.
     * @param[in] appconfig Configuration that describes the application's behavior.
     */
    explicit app(cfg::config& appconfig)
        : appcfg{&appconfig} {}

    /**
     * @brief Core mechanics of this application.
     * @{
     */
    auto setup() -> void override;
    auto exit() -> void override;
    auto update() -> void override;
    auto draw() -> void override;
    /** @} */

    /**
     * @brief Event-based mechanics of this application.
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
     * @brief Drawing mechanics.
     * @param[in] x Window coordinate along the x-axis.
     * @param[in] y Window coordinate along the y-axis.
     * @{
     */
    auto draw_camera(float x, float y) const -> void;
    auto draw_menu(float x, float y) const -> void;
    auto draw_fps(float x, float y) const -> void;
    auto draw_debug() const -> void;
    /** @} */

    /**
     * @brief Menu interface mechanics.
     * @{
     */
    auto make_menu() noexcept -> void;
    auto show_menu() noexcept -> void;
    auto exit_menu() noexcept -> void;
    /** @} */

    /**
     * @brief Handling of user-input events.
     * @param[in] key Key-code of the pressed key.
     * @param[in] x Mouse position along the x-axis.
     * @param[in] y Mouse position along the y-axis.
     * @{
     */
    auto handle_key_event(int key) noexcept -> void;
    auto handle_menu_event(int key) noexcept -> void;
    auto handle_input_event(int key) -> void;
    auto handle_mouse_event(int x, int y) -> void;
    /** @} */
    
    /**
     * @brief Value input mechanics.
     * @param[in] key Key-code of the pressed key.
     * @{
     */
    auto apply_input_value() -> void;
    auto erase_input_value() noexcept -> void;
    auto add_input_value(unsigned char key) noexcept -> void;
    /** @} */

    /**
     * @brief Selects a menu option, if it exists.
     * @param[in] key Key-code of the pressed key.
     */
    auto select_option(unsigned char key) noexcept -> void;

    /**
     * @brief Starts a connection with a serial device.
     * @details The device ID can be set in the configuration file.
     */
    auto start_serial() -> void;

    /**
     * @brief Tracks the position of the ball.
     * @details Applies a computer vision algorithm to the camera feed.
     */
    auto track_ball() -> void;

    /**
     * @brief Controls the PID values.
     * @details Calculates the required angles for the servo controller based on the
     *     current and historic positions of the ball while taking previously applied
     *     correction into account.
     */
    auto control_pid() -> void;

    /**
     * @brief Generates transformation matrices for the given servo axis.
     * @param[in] axis Servo axis for which the matrix will be generated.
     */
    auto genTransMatrix(int axis) -> void;

    /**
     * @brief Calibration mechanics.
     * @details Provides a mapping between the coordinates of the camera feed and the
     *     axes of the servo motors.
     * @param[in] x Mouse position along the x-axis.
     * @param[in] y Mouse position along the y-axis.
     * @{
     */
    auto calibrate(int x, int y) -> void;
    auto recalibrate() -> void;
    auto finishCalibration() -> void;
    /** @} */

    /**
     * @brief Setpoint mechanics.
     * @details Allows the user to define a new setpoint to which the ball should be
     *     positioned in real time.
     * @param[in] x Mouse position along the x-axis.
     * @param[in] y Mouse position along the y-axis.
     * @{
     */
    auto setSetPoint(int x, int y) -> void;
    auto updateSetPoint() -> void;
    /** @} */

    /**
     * @enum appstate
     * @brief Global state of the application.
     */
    enum class appstate {
        running,    /**< Running the application. */
        calibration /**< Calibrating the camera and servo motors. */
    };

    /**
     * @enum inputstate
     * @brief Relates to user input. 
     */
    enum class inputstate {
        app,  /**< Waiting for application input. */
        menu, /**< Selecting a menu option. */
        value /**< Entering a new value for a setting. */
    };

    /**
     * @typedef matrix_type
     * @brief Container type for position vectors.
     * @tparam N Number of position vectors.
     */
    template<std::size_t N>
    using matrix_type = std::array<ofPoint, N>;

    util::access_ptr<cfg::config> appcfg; /**< Application configuration. */
    ofSerial serial;                      /**< Serial connection. */
    cam::devptr camera;                   /**< PS3 Eye camera. */
    cam::frame_info camstats;             /**< Camera statistics. */
    std::unique_ptr<uint8[]> camframe;    /**< Live camera frame. */
    cv::Mat frame;                        /**< Transformed camera frame. */

    ui::menu<cfg::cfgitem, std::function<void()>> cfgmenu; /**< Configuration menu. */
    inputstate inputmode{inputstate::app}; /**< User input mode. */
    std::string inputvalue;                /**< Input value buffer. */
    std::string valueprompt;               /**< Input value interface. */
    std::string menuprompt;                /**< Menu interface. */

    struct {
        double kp; /**< Proportional gain. */
        double ki; /**< Integral gain. */
        double kd; /**< Derivative gain. */
    } pid;         /**< PID controller values. */

    struct {
        int min;  /**< Minimum ball radius. */
        int max;  /**< Maximum ball radius. */
    } ballradius; /**< Ball radius values. */

    appstate appmode = appstate::calibration; /**< Global application state. */
    int pointsCalibrated{0};                  /**< Calibrated points counter. */

    matrix_type<3> calibrationPoints;     /**< Servo positions. */
    matrix_type<3> transMatricesPreScale; /**< Transformation matrices pre-scaling. */
    matrix_type<3> transMatrices;         /**< Transformation matrices post-scaling. */

    float targetScale;    /**< Approximated target setup scale in mm. */
    ofPoint targetCenter; /**< Approximated target setup center point in mm. */
    ofPoint centerPoint;  /**< Center of the calibration points. */

    ofPoint ballPos;         /**< Ball position. */
    ofPoint setPoint;        /**< Setpoint position. */
    ofPoint oldSetPoint;     /**< Previous setpoint position. */
    ofPoint newSetPoint;     /**< Future setpoint position. */
    ofTime startTime;        /**< Setpoint movement start time. */
    double moveTimeSec{1.0}; /**< Setpoint total movement time. */

    std::array<float, 3> ballPosPerAxis;  /**< Ball position per servo axis. */
    std::array<float, 3> setPointPerAxis; /**< Setpoint position per servo axis. */

    std::vector<ofPolyline> debugLines;   /**< Debug visualization lines. */
    std::vector<ofColor> debugLineColors; /**< Debug visualization line colors. */

    std::array<double, 3> prevError{0.0}; /**< Previous ball position error. */
    std::array<double, 3> iError{0.0};    /**< Current ball position error. */
    std::array<std::array<double, 5>, 3> servoAction{0.0}; /**< Servo angles. */
    int servoActI{0}; /**< Servo action index for the moving average filter. */
};

} // namespace of

#endif
