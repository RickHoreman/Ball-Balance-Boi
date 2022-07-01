/**
 * @file       camera.cpp
 * @version    0.1
 * @date       May 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 * 
 * @brief Implementation of the PS3 Eye camera interface.
 */

#include "camera.h"

#include "ps3eye.h"
#include <ofUtils.h>

/**
 * @namespace cam
 * @brief Camera related components.
 */
namespace cam {

/**
 * @copydoc frame_info::update
 */
auto frame_info::update() -> void {
    ++count;

    auto const time = ofGetElapsedTimeMillis();
    if (time < (sampletime + decltype(time){1'000})) return;

    fps_ = count / ((time - sampletime) * 0.001f);
    sampletime = time;
    count = 0;
}

/**
 * @copydoc get_device
 */
auto get_device(devlist::size_type device_id) -> devptr {
    auto const& devices = ps3cam::getDevices();
    if (device_id >= devices.size()) {
        throw camera_error{"could not find ps3 camera"};
    }
    return devices[device_id];
}

} // namespace cam
