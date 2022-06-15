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
    if (device_id >= devices.size()) {
        throw camera_error{"could not find ps3 camera"};
    }
    return devices[device_id];
}

} // namespace cam
