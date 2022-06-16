/**
 * @file       config.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief ..
 * @details ..
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
#include <format>
#include <string>
#include <string_view>
#include <tuple>
#include <type_traits>

 /**
  * @namespace ..
  * @brief ..
  */
namespace cfg {

/**
 * @enum ..
 * @brief ..
 * @details ..
 */
enum class dtype : uint8 {
    boolean, /**< .. */
    uint8,   /**< .. */
    uint16,  /**< .. */
    int32,   /**< .. */
    fraction /**< .. */
};

/**
 * @brief ..
 * @details ..
 * @tparam
 */
template<typename>
struct dtype_from;

template<>
struct dtype_from<bool>
    : std::integral_constant<dtype, dtype::boolean> {};

template<>
struct dtype_from<uint8>
    : std::integral_constant<dtype, dtype::uint8> {};

template<>
struct dtype_from<uint16>
    : std::integral_constant<dtype, dtype::uint16> {};

template<>
struct dtype_from<int32>
    : std::integral_constant<dtype, dtype::int32> {};

template<>
struct dtype_from<double>
    : std::integral_constant<dtype, dtype::fraction> {};
/** @} */

/**
 * @brief ..
 * @details ..
 * @tparam ..
 */
template<dtype>
struct typename_from;

template<>
struct typename_from<dtype::boolean>
    : std::type_identity<bool> {};

template<>
struct typename_from<dtype::uint8>
    : std::type_identity<uint8> {};

template<>
struct typename_from<dtype::uint16>
    : std::type_identity<uint16> {};

template<>
struct typename_from<dtype::int32>
    : std::type_identity<int32> {};

template<>
struct typename_from<dtype::fraction>
    : std::type_identity<double> {};
/** @} */

/**
 * @class ..
 * @brief ..
 * @details ..
 * @tparam ..
 */
template<typename Value>
    requires cc::arithmetic<Value>
class config_item {
public:
    /**
     * @brief ..
     */
    config_item() = default;

    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @param[in] .. ..
     */
    template<typename T>
        requires cc::arithmetic<T> or std::is_enum_v<T>
    constexpr config_item(std::string name, dtype type, T value = {}):
        name_{std::move(name)},
        value_{type, static_cast<Value>(value)}
    {}

    /**
     * @brief ..
     * @return ..
     */
    [[nodiscard]]
    constexpr auto tagname() const noexcept -> std::string {
        auto tag = std::string{name_};
        std::replace(tag.begin(), tag.end(), ' ', '-');
        return tag;
    }

    /**
     * @brief ..
     * @return ..
     */
    [[nodiscard]]
    constexpr auto name() const noexcept -> std::string const&
    { return name_; }

    
    /**
     * @brief ..
     * @details ..
     * @return ..
     * @{
     */
    [[nodiscard]]
    constexpr auto& value() noexcept
    { return value_; }

    [[nodiscard]]
    constexpr auto const& value() const noexcept
    { return value_; }
    /** @} */

    /**
     * @brief ..
     * @details ..
     * @return ..
     */
    [[nodiscard]]
    constexpr auto to_string() const -> std::string {
        return value().type() == dtype::boolean
            ? std::format("{:>16}", value().get() ? "on" : "off")
            : std::format("{:16}", value().get());
    }

    /**
     * @brief ..
     * @details ..
     * @return ..
     */
    template<cc::arithmetic T>
    [[nodiscard]]
    constexpr auto to() const noexcept -> T
    { return static_cast<T>(value().get()); }

    /**
     * @brief ..
     * @details ..
     */
    [[nodiscard]]
    constexpr operator Value() const noexcept
    { return value().get(); }

    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(config_item const&, config_item const&) -> bool = default;

private:
    /**
     * @struct ..
     * @brief ..
     * @details ..
     */
    class config_value {
    public:
        /**
         * @brief ..
         */
        config_value() = default;

        /**
         * @brief ..
         * @details ..
         * @param[in] .. ..
         * @param[in] .. ..
         */
        constexpr config_value(dtype type, Value value)
            : type_{type}, value_{value} {}

        /**
         * @brief ..
         * @details ..
         * @tparam ..
         * @param[in] .. ..
         */
        template<std::invocable<config_item const&, Value> Action>
        constexpr auto dispatch_type(Action const& action, config_item const& item)
        const noexcept -> decltype(auto) {
            switch (type_) {
            case dtype::boolean:  return std::invoke(action, item, to<dtype::boolean>());
            case dtype::uint8:    return std::invoke(action, item, to<dtype::uint8>());
            case dtype::uint16:   return std::invoke(action, item, to<dtype::uint16>());
            case dtype::int32:    return std::invoke(action, item, to<dtype::int32>());
            case dtype::fraction: return std::invoke(action, item, to<dtype::fraction>());
            }
        }

        /**
         * @brief ..
         * @details ..
         * @tparam ..
         */
        template<dtype type>
        [[nodiscard]]
        constexpr auto to() const noexcept
        { return static_cast<typename_from<type>::type>(value_); }

        /**
         * @brief ..
         * @return ..
         */
        [[nodiscard]]
        constexpr auto type() const noexcept -> dtype
        { return type_; }

        /**
         * @brief ..
         * @return ..
         */
        [[nodiscard]]
        constexpr auto get() const noexcept -> Value
        { return value_; }

        /**
         * @brief ..
         * @details ..
         * @tparam ..
         * @param[in] .. ..
         * @return ..
         */
        constexpr auto set(Value value) noexcept -> void
        { value_ = value; }

        /**
         * @brief ..
         * @details .. attempts to set value type with index 1 when empty
         * @tparam ..
         * @param[in] .. ..
         * @return ..
         */
        constexpr auto set(std::string_view value) -> void {
            auto const from_chars = [this, value](auto result) {
                std::from_chars(value.data(), value.data() + value.size(), result);
                value_ = result;
            };
            switch (type_) {
            case dtype::boolean:
                from_chars(to<dtype::uint8>());
                value_ = to<dtype::boolean>();
                return;
            case dtype::uint8:    return from_chars(to<dtype::uint8>());
            case dtype::uint16:   return from_chars(to<dtype::uint16>());
            case dtype::int32:    return from_chars(to<dtype::int32>());
            case dtype::fraction: return from_chars(to<dtype::fraction>());
            }
        }

        /**
         * @brief ..
         */
        [[nodiscard]]
        friend auto operator==(config_value const&, config_value const&) -> bool = default;

    private:
        Value value_{};                   /**< .. */
        dtype type_{dtype_from<Value>{}}; /**< .. */
    };

    std::string name_;   /**< .. */
    config_value value_; /**< .. */
};

/**
 * @typedef ..
 * @brief ..
 */
template<typename Value = double>
using cfgitem = config_item<Value>;

/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct xmlcfg {
    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(xmlcfg const&, xmlcfg const&) -> bool = default;

    ofxXmlSettings file;  /**< .. */
    std::string filename; /**< .. */
    std::string tagname;  /**< .. */
};


/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct screencfg {
    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(screencfg const&, screencfg const&) -> bool = default;

    cfgitem<> width;  /**< .. */
    cfgitem<> height; /**< .. */
    cfgitem<> rate;   /**< .. */
};

/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct pidcfg {
    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(pidcfg const&, pidcfg const&) -> bool = default;

    cfgitem<> kp; /**< .. */
    cfgitem<> ki; /**< .. */
    cfgitem<> kd; /**< .. */
};

/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct serialcfg {
    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(serialcfg const&, serialcfg const&) -> bool = default;

    cfgitem<> enabled;  /**< .. */
    cfgitem<> comport;  /**< .. */
    cfgitem<> baudrate; /**< .. */
};

/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct visioncfg {
    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(visioncfg const&, visioncfg const&) -> bool = default;

    cfgitem<> trackball; /**< .. */
    cfgitem<> minballradius; /**< .. */
    cfgitem<> maxballradius; /**< .. */
};

/**
 * @struct frame
 * @brief ..
 */
struct framecfg {
    /**
     * @brief ..
     * @details ..
     * @param[in] depth ..
     * @return ..
     */
    [[nodiscard]]
    constexpr auto size(unsigned depth = 1) const noexcept -> unsigned
    { return depth * width * height; }

    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(framecfg const&, framecfg const&) -> bool = default;

    cfgitem<> width;  /**< .. */
    cfgitem<> height; /**< .. */
    cfgitem<> rate;   /**< .. */
};

/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct balancecfg {
    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(balancecfg const&, balancecfg const&) -> bool = default;

    cfgitem<> red;       /**< .. */
    cfgitem<> green;     /**< .. */
    cfgitem<> blue;      /**< .. */
    cfgitem<> autowhite; /**< .. */
};

/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct camcfg {
    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(camcfg const&, camcfg const&) -> bool = default;

    framecfg frame;       /**< .. */
    balancecfg balance;   /**< .. */
    cfgitem<> format;     /**< .. */
    cfgitem<> exposure;   /**< .. */
    cfgitem<> sharpness;  /**< .. */
    cfgitem<> contrast;   /**< .. */
    cfgitem<> brightness; /**< .. */
    cfgitem<> hue;        /**< .. */
    cfgitem<> gain;       /**< .. */
    cfgitem<> autogain;   /**< .. */
};

/**
 * @struct ..
 * @brief ..
 * @details ..
 */
struct config {
    /**
     * @brief ..
     * @return ..
     */
    [[nodiscard]]
    static auto defaults() noexcept -> config {
        return {
            .xml{
                .filename{"settings.xml"},
                .tagname{"settings"}},
            .screen{
                .width{"screen width", dtype::uint16, 800},
                .height{"screen height", dtype::uint16, 600},
                .rate{"screen rate", dtype::uint16, 60}},
            .serial{
                .enabled{"serial enabled", dtype::boolean, true},
                .comport{"com port", dtype::uint8, 7},
                .baudrate{"baudrate", dtype::int32, 115'200}},
            .pid{
                .kp{"proportional", dtype::fraction, 0.3},
                .ki{"integral", dtype::fraction, 0.001},
                .kd{"derivative", dtype::fraction, 5.0}},
            .vision{
                .trackball{"ball tracking", dtype::boolean, true},
                .minballradius{"min. ball radius", dtype::uint16, 5},
                .maxballradius{"max. ball radius", dtype::uint16, 75}},
            .cam{
                .frame{
                    .width{"frame width", dtype::uint16, 640},
                    .height{"frame height", dtype::uint16, 480},
                    .rate{"frame rate", dtype::uint16, 60}},
                .balance{
                    .red{"red balance", dtype::uint8, 128},
                    .green{"green balance", dtype::uint8, 128},
                    .blue{"blue balance", dtype::uint8, 128},
                    .autowhite{"auto white bal.", dtype::boolean, false}},
                .format{"color format", dtype::uint8, ::cam::format::Gray},
                .exposure{"exposure", dtype::uint8, 20},
                .sharpness{"sharpness", dtype::uint8, 128},
                .contrast{"contrast", dtype::uint8, 128},
                .brightness{"brightness", dtype::uint8, 128},
                .hue{"hue", dtype::uint8, 128},
                .gain{"gain", dtype::uint8, 20},
                .autogain{"auto gain", dtype::boolean, false}}
        };
    }

    /**
     * @brief ..
     * @details ..
     */
    [[nodiscard]]
    constexpr auto to_tuple() noexcept {
        return std::tie(
            screen.width,
            screen.height,
            screen.rate,
            serial.enabled,
            serial.comport,
            serial.baudrate,
            pid.kp,
            pid.ki,
            pid.kd,
            vision.trackball,
            vision.minballradius,
            vision.maxballradius,
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
     * @brief ..
     * @details ..
     */
    auto loadxml() -> void {
        xml.file.load(xml.filename);
        xml.file.addTag(xml.tagname);
        xml.file.pushTag(xml.tagname);
        std::apply([this](auto&... item) {
            ((item.value().set(
                xml.file.getValue(item.tagname(), item.value().get()))), ...);
            }, to_tuple());
        xml.file.popTag();
        xml.file.clear();
    }

    /**
     * @brief ..
     * @details ..
     */
    auto savexml() -> void {
        xml.file.addTag(xml.tagname);
        xml.file.pushTag(xml.tagname);
        std::apply([this](auto&... items) {
            auto const xmlsetter = [&](auto const& item, auto value)
                { xml.file.setValue(item.tagname(), value); };
            ((items.value().dispatch_type(xmlsetter, items)), ...);
        }, to_tuple());
        xml.file.saveFile(xml.filename);
        xml.file.popTag();
        xml.file.clear();
    }

    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(config const&, config const&) -> bool = default;

    xmlcfg xml;       /**< .. */
    screencfg screen; /**< .. */
    serialcfg serial; /**< .. */
    pidcfg pid;       /**< .. */
    visioncfg vision; /**< .. */
    camcfg cam;       /**< .. */
};

} // namespace cfg

#endif
