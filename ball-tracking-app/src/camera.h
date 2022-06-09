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

#ifndef BBB_CAM_CAMERA_H
#define BBB_CAM_CAMERA_H

#include "ps3eye.h"

#include <cstdint>
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
 * @enum status
 * @brief ..
 */
enum class status {
    operational,  /**< .. */
    init_failure  /**< .. */
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
     */
    framestats() = default;

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
    constexpr auto fps() const noexcept -> float
    { return fps_; }

    /**
     * @brief ..
     */
    friend auto operator<=>(framestats const&, framestats const&) = default;

private:
    std::uint64_t sampletime{};   /**< .. */
    std::uint16_t samplecount{};  /**< .. */
    std::uint16_t count{};        /**< .. */
    float fps_{};                 /**< .. */
};

/**
 * @namespace ..
 * @brief ..
 */
namespace cfg {

/**
* @typedef format
* @brief ..
*/
using format = ps3cam::EOutputFormat;

/**
 * @struct frame
 * @brief ..
 */
struct frame {
    /**
     * @brief ..
     * @details ..
     * @param[in] depth ..
     * @return ..
     */
    constexpr auto size(unsigned depth = 1) const noexcept -> unsigned
    { return depth * width * height; }

    /**
     * @brief ..
     */
    friend auto operator<=>(frame const&, frame const&) = default;

    std::uint16_t width;   /**< .. */
    std::uint16_t height;  /**< .. */
    std::uint16_t rate;    /**< .. */
};

/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct balance {
    /**
     * @brief ..
     */
    friend auto operator<=>(balance const&, balance const&) = default;

    std::uint8_t red;    /**< .. */
    std::uint8_t green;  /**< .. */
    std::uint8_t blue;   /**< .. */
    bool autowhite;      /**< .. */
};

} // namespace cfg

/**
 * @struct config
 * @brief ..
 */
struct config {
    /**
     * @brief ..
     * @return ..
     */
    static constexpr auto defaults() noexcept -> config {
        return {
            .format{cfg::format::Gray},
            .frame{.width{640},.height{480},.rate{75}},
            .balance{.red{128},.green{128},.blue{128},.autowhite{false}},
            .sharpness{0},
            .exposure{30},
            .brightness{150},
            .contrast{150},
            .gain{20},
            .hue{143},
            .autogain{false}
        };
    }

    /**
     * @brief ..
     */
    friend auto operator<=>(config const&, config const&) = default;

    cfg::format format;       /**< .. */
    cfg::frame frame;         /**< .. ? */
    cfg::balance balance;     /**< .. ? */
    std::uint8_t sharpness;   /**< .. */
    std::uint8_t exposure;    /**< .. */
    std::uint8_t brightness;  /**< .. */
    std::uint8_t contrast;    /**< .. */
    std::uint8_t gain;        /**< .. */
    std::uint8_t hue;         /**< .. */
    bool autogain;            /**< .. */
};

/**
 * @brief ..
 * @details ..
 * @param[in] device_id ..
 * @return ..
 */
auto getdevice(devlist::size_type device_id = 0) -> devptr;

/**
 * @brief ..
 * @details ..
 * @param[in] camera ..
 * @param[in] camcfg ..
 * @return ..
 */
auto initcamera(ps3cam& camera, config const& camcfg) -> status;

} // namespace cam

#endif
