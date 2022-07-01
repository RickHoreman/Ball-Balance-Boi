/**
 * @file       camera.h
 * @version    0.1
 * @date       May 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief Interface for the PS3 Eye camera.
 */

#ifndef CAM_CAMERA_H
#define CAM_CAMERA_H

#include "ps3eye.h"
#include "types.h"

#include <stdexcept>
#include <type_traits>

/**
 * @namespace cam
 * @brief Camera related components.
 */
namespace cam {

/**
 * @typedef ps3cam
 * @brief PS3 Eye camera object type.
 */
using ps3cam = ps3eye::PS3EYECam;

/**
 * @typedef devptr
 * @brief PS3 Eye camera object pointer type.
 */
using devptr = ps3cam::PS3EYERef;

/**
 * @typedef devlist
 * @brief Container type for PS3 Eye camera objects.
 */
using devlist = std::remove_reference_t<
    decltype(ps3cam::getDevices())>;

/**
 * @typedef format
 * @brief Image format type for the PS3 Eye camera.
 */
using format = ps3cam::EOutputFormat;

/**
 * @struct camera_error
 * @brief Exception related to camera connections.
 */
struct camera_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * @class frame_info
 * @brief Provides information about the frames of the PS3 Eye camera.
 */
class frame_info {
public:
    /**
     * @brief Updates the frame rate counter.
     * @details This function should be called for each new camera frame.
     */
    auto update() -> void;

    /**
     * @brief Returns the frame rate of the camera.
     */
    [[nodiscard]]
    constexpr auto fps() const noexcept -> float
    { return fps_; }

    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(frame_info const&, frame_info const&) -> bool = default;

private:
    uint64 sampletime{}; /**< Timestamp since the last calculated frame rate. */
    uint16 count{};      /**< Number of frames since the last update. */
    float fps_{};        /**< Frames per second. */
};

/**
 * @brief Gets a PS3 Eye camera with the given device ID.
 * @param[in] device_id Device ID of the camera to retrieve. Defaulted to 0.
 * @exception camera_error Throws an exception when the camera could not be found.
 * @return Pointer to the camera object.
 */
[[nodiscard]]
auto get_device(devlist::size_type device_id = 0) -> devptr;

/**
 * @brief Starts the given PS3 Eye camera with a camera configuration.
 * @param[in] camera Camera object to initialize and start.
 * @param[in] camcfg Contains the configuration of the camera.
 * @exception camera_error Throws an exception when the camera could not be initialized.
 */
auto start_camera(ps3cam& camera, auto const& camcfg) -> void {
    auto const is_initialized = camera.init(
        camcfg.frame.width,
        camcfg.frame.height,
        camcfg.frame.rate,
        static_cast<format>(static_cast<int>(camcfg.format))
    );
    if (not is_initialized) {
        throw camera_error{"could not initialize ps3 camera"};
    }
    camera.setRedBalance(camcfg.balance.red);
    camera.setGreenBalance(camcfg.balance.green);
    camera.setBlueBalance(camcfg.balance.blue);
    camera.setAutoWhiteBalance(camcfg.balance.autowhite);   
    camera.setExposure(camcfg.exposure);
    camera.setSharpness(camcfg.sharpness);
    camera.setBrightness(camcfg.brightness);
    camera.setContrast(camcfg.contrast);
    camera.setGain(camcfg.gain);
    camera.setHue(camcfg.hue);
    camera.setAutogain(camcfg.autogain);
    camera.start();
}

} // namespace cam

#endif
