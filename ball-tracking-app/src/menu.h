/**
 * @file       menu.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief ..
 * @details ..
 */

#ifndef OPT_MENU_H
#define OPT_MENU_H

#include "memory.h"
#include "traits.h"

#include <cctype>
#include <concepts>
#include <format>
#include <functional>
#include <string>
#include <unordered_map>
#include <utility>

/**
 * @namespace ..
 * @brief ..
 */
namespace opt {

/**
 * @class ..
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 */
template<typename ConfigItem, std::invocable<ConfigItem> Action>
class menu_item {
public:
    /**
     * @brief ..
     */
    menu_item() = default;

    /**
     * @brief ..
     * @details ..
     * @param[in] .. ..
     */
    constexpr explicit menu_item(ConfigItem& cfgitem)
        : value_{cfgitem} {}

    /**
     * @brief ..
     * @details ..
     * @param[in] .. ..
     */
    constexpr explicit menu_item(Action action)
        : value_{std::move(action)} {}

    /**
     * @brief ..
     * @details ..
     * @param[in] .. ..
     */
    constexpr menu_item(ConfigItem& cfgitem, Action action)
        : value_{cfgitem, std::move(action)} {}

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
    constexpr auto to_string() const -> std::string
    { return std::format("{:20} {}\n", value().get().name(), value().get().to_string()); }

    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(menu_item const&, menu_item const&) -> bool = default;

private:
    /**
     * @struct ..
     * @brief ..
     * @details ..
     */
    class menu_value {
    public:
        /**
         * @brief ..
         */
        menu_value() = default;

        /**
         * @brief ..
         * @details ..
         * @param[in] .. ..
         */
        constexpr explicit menu_value(ConfigItem& cfgitem)
            : cfgitem_{&cfgitem} {}

        /**
         * @brief ..
         * @details ..
         * @param[in] .. ..
         */
        constexpr explicit menu_value(Action action)
            : action_{std::move(action)} {}

        /**
         * @brief ..
         * @details ..
         * @param[in] .. ..
         * @param[in] .. ..
         */
        constexpr menu_value(ConfigItem& cfgitem, Action action):
            cfgitem_{&cfgitem},
            action_{std::move(action)}
        {}

        /**
         * @brief ..
         * @details ..
         * @return
         */
        constexpr auto apply() const -> decltype(auto)
        { return std::invoke(action_, cfgitem_ == nullptr ? ConfigItem{} : *cfgitem_); }

        /**
         * @brief ..
         * @details ..
         * @tparam ..
         * @param[in] .. ..
         * @return ..
         */
        template<typename T>
        constexpr auto apply(T&& value) -> void {
            set(std::forward<T>(value));
            if (not action_) return;
            std::invoke(action_, *cfgitem_);
        }

        /**
         * @brief ..
         * @return ..
         */
        [[nodiscard]]
        constexpr auto get() const noexcept -> ConfigItem const&
        { return *cfgitem_; }

        /**
         * @brief ..
         * @details ..
         * @param[in] .. ..
         */
        template<typename T>
        constexpr auto set(T&& value) noexcept -> void {
            if (cfgitem_ == nullptr) return;
            cfgitem_->value().set(std::forward<T>(value));
        }

        /**
         * @brief ..
         */
        [[nodiscard]]
        friend auto operator==(menu_value const&, menu_value const&) -> bool = default;

    private:
        mem::access_ptr<ConfigItem> cfgitem_{nullptr}; /**< .. */
        Action action_;                                /**< .. */
    };

    menu_value value_; /**< .. */
};

/**
 * @class ..
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 */
template<typename ConfigItem, std::invocable<ConfigItem> Action>
class menu {
    /**
     * @typedef ..
     * @brief ..
     */
    using item_type = menu_item<ConfigItem, Action>;

public:
    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @param[in] .. ..
     */
    template<typename... Ts>
    constexpr auto add(unsigned char key, Ts&&... args) -> void
    { items.try_emplace(key, std::forward<Ts>(args)...); }

    /**
     * @brief ..
     * @details ..
     * @param[in] .. ..
     * @return ..
     */
    [[nodiscard]]
    constexpr auto contains(unsigned char key) const noexcept -> bool
    { return items.contains(key); }

    /**
     * @brief ..
     * @details ..
     * @param[in] .. ..
     */
    constexpr auto select(unsigned char key) noexcept -> void
    { selection_ = key; }

    /**
     * @brief ..
     * @details ..
     * @param[in] .. ..
     * @return ..
     */
    [[nodiscard]]
    constexpr auto selection() -> item_type&
    { return items.at(selection_); }

    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @return ..
     */
    [[nodiscard]]
    constexpr auto to_string() const -> std::string {
        auto result = std::string{};
        auto const max_item_name = 32;
        result.reserve(max_item_name * items.size());

        for (auto const& [key, item] : items) {
            result += std::format("{:c} | {}",
                std::toupper(key), item.to_string());
        }
        return result;
    }

    /**
     * @brief ..
     */
    [[nodiscard]]
    friend auto operator==(menu const&, menu const&) -> bool = default;

private:
    using map_type = std::unordered_map<unsigned char, item_type>; /**< .. */

    map_type items;             /**< .. */
    unsigned char selection_{}; /**< .. */
};

} // namespace opt

#endif
