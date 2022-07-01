/**
 * @file       config.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief Configuration settings of the ball-tracking application.
 */

#ifndef CFG_CONFIG_H
#define CFG_CONFIG_H

#include "camera.h"
#include "concepts.h"
#include "types.h"
#include "utility.h"

#include <ofxXmlSettings.h>

#include <algorithm>
#include <charconv>
#include <concepts>
#include <format>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>

/**
 * @namespace cfg
 * @brief Configuration related components.
 */
namespace cfg {

/**
 * @class config_item
 * @brief Configuration item.
 * @details Maps an arithmetic value to a named setting.
 * @tparam Values One or more arithmetic types, requried to be distinct.
 */
template<cc::arithmetic... Values>
    requires cc::distinct<Values...>
class config_item {
public:
    /**
     * @brief Default constructs a configuration item.
     */
    config_item() = default;

    /**
     * @brief Constructs a configuration item with the given name and optional value.
     * @tparam Value Value-type of the value to be stored.
     * @param[in] name Name of the configuration item.
     * @param[in] value Initial value of the configuration item. Value-initializes the
     *     first value-type by default.
     */
    template<cc::convertible_to_any<Values...> Value>
    constexpr explicit config_item(std::string name, Value value = {}):
        name_{std::move(name)},
        value_{value}
    {}

    /**
     * @brief Returns the name of the configuration item.
     */
    [[nodiscard]]
    constexpr auto name() const noexcept -> std::string const&
    { return name_; }

    /**
     * @brief Returns the tag name of the configuraiton item.
     * @details Makes a copy of the name and simply replaces any space with a hyphen.
     */
    [[nodiscard]]
    constexpr auto tagname() const noexcept -> std::string {
        auto tag = std::string{name_};
        std::ranges::replace(tag, ' ', '-');
        return tag;
    }

    /**
     * @brief Returns the stored value converted to a string.
     * @tparam String Conversion-type, required to be a std::string.
     */
    template<std::same_as<std::string> String>
    [[nodiscard]]
    constexpr auto to() const -> String
    { return std::visit([](auto value) { return std::format("{}", value); }, value_); }

    /**
     * @brief Returns the stored value.
     * @tparam Value Type of the value to retrieve, required to match one of the
     *     instantiated value types.
     */
    template<cc::same_as_any<Values...> Value>
    [[nodiscard]]
    constexpr auto to() const -> Value
    { return std::get<Value>(value_); }

    /**
     * @brief Sets a new value to a configuration item, represented as a string.
     * @details Conversion to a boolean value will only result to true when the given
     *     string matches "1" or "true".
     * @param[in] value String that represents an arithmetic value to set.
     */
    constexpr auto set(std::string_view value) noexcept -> void {
         std::visit(util::visitor{
            [value](bool& value_ref) { value_ref = (value == "1" or value == "true"); },
            [value](auto& value_ref)
            { std::from_chars(value.data(), value.data() + value.size(), value_ref); }
        }, value_);
    }

    /**
     * @brief Sets a new value to a configuration item.
     * @param[in] value Arithmetic value to set, required to be convertible to one of the
     *     instantiated value types.
     */
    constexpr auto set(cc::convertible_to_any<Values...> auto value) noexcept -> void
    { value_ = value; }

    /**
     * @brief Contextually converts a configuration item to its stored boolean value.
     */
    [[nodiscard]]
    constexpr explicit operator bool() const noexcept
        requires cc::same_as_any<bool, Values...>
    { return std::get<bool>(value_); }

    /**
     * @brief Implicitly converts a configuration item to its stored value.
     * @tparam Value Type of the value to retrieve, required to be convertible to one of
     *     the instantiated value types.
     */
    template<cc::convertible_to_any<Values...> Value>
    [[nodiscard]]
    constexpr operator Value() const noexcept
    { return std::visit([](auto value) { return static_cast<Value>(value); }, value_); }

    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(config_item const&, config_item const&) -> bool = default;

private:
    std::string name_;              /**< Name of the setting. */
    std::variant<Values...> value_; /**< Arithmetic value to store. */
};

/**
 * @typedef cfgitem
 * @brief Configuration item that contains an arithmetic value
 *     of either bool, uint8, int, or double.
 */
using cfgitem = config_item<bool, uint8, int, double>;

/**
 * @struct xmlcfg
 * @brief XML related configuration.
 */
struct xmlcfg {
    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(xmlcfg const&, xmlcfg const&) -> bool = default;

    ofxXmlSettings file;  /**< XML file. */
    std::string filename; /**< Name of the XML file. */
    std::string tagname;  /**< Top-level tag name. */
};

/**
 * @struct screencfg
 * @brief Application screen related configuration.
 */
struct screencfg {
    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(screencfg const&, screencfg const&) -> bool = default;

    cfgitem width;  /**< Width of the application screen. */
    cfgitem height; /**< Height of the application screen. */
    cfgitem rate;   /**< Frame rate of the application screen. */
};

/**
 * @struct pidcfg
 * @brief PID controller related configuration.
 */
struct pidcfg {
    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(pidcfg const&, pidcfg const&) -> bool = default;

    cfgitem kp; /**< Proportional gain. */
    cfgitem ki; /**< Integral gain. */
    cfgitem kd; /**< Derivative gain. */
};

/**
 * @struct serialcfg
 * @brief Serial connection related configuration.
 */
struct serialcfg {
    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(serialcfg const&, serialcfg const&) -> bool = default;

    cfgitem enabled;  /**< Enables a serial connection. */
    cfgitem deviceid; /**< Device ID of the serial device. */
    cfgitem baudrate; /**< Baudrate of the serial connection. */
};

/**
 * @struct rangecfg
 * @brief Range related configuration.
 */
struct rangecfg {
    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(rangecfg const&, rangecfg const&) -> bool = default;

    cfgitem min; /**< Minimum range value. */
    cfgitem max; /**< Maximum range value. */
};

/**
 * @struct visioncfg
 * @brief Computer vision related configuration.
 */
struct visioncfg {
    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(visioncfg const&, visioncfg const&) -> bool = default;

    cfgitem displaydebug; /**< Draws debug visualization lines. */
    cfgitem trackball;    /**< Enables tracking of the ball. */
    rangecfg ballradius;  /**< Radius of the ball. */
};

/**
 * @struct framecfg
 * @brief Camera frame related configuration.
 */
struct framecfg {
    /**
     * @brief Returns the size of the camera frame.
     * @param[in] depth Depth of the camera frame, required to be an arithmetic type.
     */
    [[nodiscard]]
    constexpr auto size(cc::arithmetic auto depth = 1) const noexcept
    { return depth * width.to<int>() * height.to<int>(); }

    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(framecfg const&, framecfg const&) -> bool = default;

    cfgitem width;  /**< Width of the camera frame. */
    cfgitem height; /**< Height of the camera frame. */
    cfgitem rate;   /**< Frame rate of the camera. */
};

/**
 * @struct balancecfg
 * @brief Color balance related configuration.
 */
struct balancecfg {
    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(balancecfg const&, balancecfg const&) -> bool = default;

    cfgitem red;       /**< Red color balance. */
    cfgitem green;     /**< Green color balance. */
    cfgitem blue;      /**< Blue color balance. */
    cfgitem autowhite; /**< Enables automatic white color balancing. */
};

/**
 * @struct camcfg
 * @brief Camera related configuration.
 */
struct camcfg {
    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(camcfg const&, camcfg const&) -> bool = default;

    framecfg frame;     /**< Camera frame configuration. */
    balancecfg balance; /**< Color balance configuration. */
    cfgitem format;     /**< Image color format. */
    cfgitem exposure;   /**< Image exposure. */
    cfgitem sharpness;  /**< Image sharpness. */
    cfgitem contrast;   /**< Image contrast. */
    cfgitem brightness; /**< Image brightness. */
    cfgitem hue;        /**< Image hue. */
    cfgitem gain;       /**< Image gain. */
    cfgitem autogain;   /**< Enables automatic image gain. */
};

/**
 * @struct config
 * @brief Configuration settings of this application.
 */
struct config {
    /**
     * @brief Returns the default configuration settings.
     */
    [[nodiscard]]
    static auto defaults() noexcept -> config {
        return {
            .xml{
                .filename{"settings.xml"},
                .tagname{"settings"}},
            .screen{
                .width{"screen width", 800},
                .height{"screen height", 600},
                .rate{"screen rate", 60}},
            .serial{
                .enabled{"serial enabled", true},
                .deviceid{"device id", 0},
                .baudrate{"baudrate", 115'200}},
            .pid{
                .kp{"proportional", 0.3},
                .ki{"integral", 0.001},
                .kd{"derivative", 5.0}},
            .vision{
                .displaydebug{"display debug", true},
                .trackball{"ball tracking", true},
                .ballradius{
                    .min{"min. ball radius", 5},
                    .max{"max. ball radius", 75}}},
            .cam{
                .frame{
                    .width{"frame width", 640},
                    .height{"frame height", 480},
                    .rate{"frame rate", 60}},
                .balance{
                    .red{"red balance", 128_u8},
                    .green{"green balance", 128_u8},
                    .blue{"blue balance", 128_u8},
                    .autowhite{"auto white bal.", false}},
                .format{"color format", static_cast<int>(cam::format::Gray)},
                .exposure{"exposure", 20_u8},
                .sharpness{"sharpness", 128_u8},
                .contrast{"contrast", 128_u8},
                .brightness{"brightness", 128_u8},
                .hue{"hue", 128_u8},
                .gain{"gain", 20_u8},
                .autogain{"auto gain", false}}
        };
    }

    /**
     * @brief Returns a tuple of all the configuration settings.
     * @note With static reflection, this helper function will become redundant.
     */
    [[nodiscard]]
    constexpr auto as_tuple() noexcept {
        return std::tie(
            screen.width,
            screen.height,
            screen.rate,
            serial.enabled,
            serial.deviceid,
            serial.baudrate,
            pid.kp,
            pid.ki,
            pid.kd,
            vision.displaydebug,
            vision.trackball,
            vision.ballradius.min,
            vision.ballradius.max,
            cam.frame.width,
            cam.frame.height,
            cam.frame.rate,
            cam.balance.red,
            cam.balance.blue,
            cam.balance.green,
            cam.balance.autowhite,
            cam.format,
            cam.exposure,
            cam.sharpness,
            cam.contrast,
            cam.brightness,
            cam.hue,
            cam.gain,
            cam.autogain
        );
    }

    /**
     * @brief Loads the configuration settings from an XML file.
     */
    auto loadxml() -> void {
        xml.file.load(xml.filename);
        xml.file.addTag(xml.tagname);
        xml.file.pushTag(xml.tagname);
        std::apply([this](auto&... items) {
            ((items.set(xml.file.getValue(
                items.tagname(), items.to<std::string>()))), ...);
        }, as_tuple());
        xml.file.popTag();
        xml.file.clear();
    }

    /**
     * @brief Saves the configuration settings to an XML file.
     */
    auto savexml() -> void {
        xml.file.addTag(xml.tagname);
        xml.file.pushTag(xml.tagname);
        std::apply([this](auto const&... items) {
            (xml.file.setValue(items.tagname(), items.to<std::string>()), ...);
        }, as_tuple());
        xml.file.saveFile(xml.filename);
        xml.file.popTag();
        xml.file.clear();
    }

    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(config const&, config const&) -> bool = default;

    xmlcfg xml;       /**< XML configuration. */
    screencfg screen; /**< Application screen configuration. */
    serialcfg serial; /**< Serial connection configuration. */
    pidcfg pid;       /**< PID controller configuration. */
    visioncfg vision; /**< Computer vision configuration. */
    camcfg cam;       /**< Camera configuration. */
};

} // namespace cfg

#endif
