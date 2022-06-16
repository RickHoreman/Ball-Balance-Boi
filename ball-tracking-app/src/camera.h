/**
 * @file       camera.h
 * @version    0.1
 * @date       May 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief ..
 * @details ..
 */

#ifndef CAM_CAMERA_H
#define CAM_CAMERA_H

#include "ps3eye.h"
#include "types.h"

#include <stdexcept>
#include <type_traits>

/**
 * @namespace cam
 * @brief ..
 */
namespace cam {

/**
 * @typedef ps3cam
 * @brief ..
 */
using ps3cam = ps3eye::PS3EYECam;

/**
 * @typedef devptr
 * @brief ..
 */
using devptr = ps3cam::PS3EYERef;

/**
 * @typedef devlist
 * @brief ..
 */
using devlist = std::remove_reference_t<
    decltype(ps3cam::getDevices())>;

/**
 * @typedef format
 * @brief ..
 */
using format = ps3cam::EOutputFormat;

/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct camera_error : std::runtime_error {
    using std::runtime_error::runtime_error;
};

/**
 * @class framestats
 * @brief ..
 * @details ..
 */
class framestats {
public:
    /**
     * @brief ..
     * @details ..
     */
    auto update() -> void;

    /**
     * @brief ..
     * @details ..
     * @return ..
     */
    [[nodiscard]]
    constexpr auto fps() const noexcept -> float
    { return fps_; }

    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(framestats const&, framestats const&) -> bool = default;

private:
    uint64 sampletime{};   /**< .. */
    uint16 samplecount{};  /**< .. */
    uint16 count{};        /**< .. */
    float fps_{};          /**< .. */
};

/**
 * @brief ..
 * @details ..
 * @param[in] device_id ..
 * @return ..
 */
[[nodiscard]]
auto getdevice(devlist::size_type device_id = 0) -> devptr;

/**
 * @brief ..
 * @details ..
 * @tparam ..
 * @param[in] camera ..
 * @param[in] camcfg ..
 * @return ..
 */
auto initcamera(ps3cam& camera, auto const& camcfg) -> void {
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
