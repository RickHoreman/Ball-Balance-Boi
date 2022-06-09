/**
 * @file       camera.cpp
 * @version    0.1
 * @date       May 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 * 
 * @brief ..
 * @details ..
 */

#include "camera.h"

#include "ps3eye.h"
#include <ofUtils.h>

/**
 * @namespace cam
 * @brief ..
 */
namespace cam {

/**
 * @copydoc framestats::update
 * @internal ..
 * 
 * @todo fix this
 */
auto framestats::update() -> void {
    ++count;

    auto const time = ofGetElapsedTimeMillis();
    if (time < (samplecount + decltype(time){1000})) return;

    auto const countdiff = count - samplecount;
    fps_ = countdiff / ((time - sampletime) * 0.001f);

    sampletime = time;
    samplecount = countdiff;
    count = 0;
}

/**
 * @copydoc getdevice
 * @internal ..
 */
auto getdevice(devlist::size_type device_id) -> devptr {
    auto const& devices = ps3cam::getDevices();
    return device_id < devices.size()
        ? devices[device_id]
        : devptr{};
}

/**
 * @copydoc initcamera
 * @internal ..
 */
auto initcamera(ps3cam& camera, config const& camcfg) -> status {
    auto const is_initialized = camera.init(
        camcfg.frame.width,
        camcfg.frame.height,
        camcfg.frame.rate,
        camcfg.format);
    if (not is_initialized) return status::init_failure;

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
    return status::operational;
}

} // namespace cam
