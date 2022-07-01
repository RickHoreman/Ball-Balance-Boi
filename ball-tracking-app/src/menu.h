/**
 * @file       menu.h
 * @version    0.1
 * @date       June 2022
 * @author     Joeri Kok
 * @author     Rick Horeman
 * @copyright  GPL-3.0 license
 *
 * @brief Menu interface.
 */

#ifndef UI_MENU_H
#define UI_MENU_H

#include "concepts.h"
#include "utility.h"

#include <algorithm>
#include <cctype>
#include <concepts>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * @namespace ui
 * @brief User interface related components.
 */
namespace ui {

/**
 * @class menu_option
 * @brief Menu option.
 * @details Maps a key to a configurable item and an optional action.
 * @tparam ConfigItem Configuration item type, required to be configurable.
 * @tparam Action Action type, required to be invocable without parameters.
 */
template<cc::configurable ConfigItem, std::invocable Action>
class menu_option {
public:
    /**
     * @brief Default constructs a menu option.
     */
    menu_option() = default;

    /**
     * @brief Constructs a menu option with the given key, configuration item, and
     *     optional action.
     * @param[in] key Key to uniquely identify a menu option.
     * @param[in] cfgitem Existing configuration item to set when applying a menu option.
     * @param[in] action Optional action to invoke when applying a menu option. Defaulted
     *     to an empty action.
     */
    constexpr menu_option(
        unsigned char key,
        ConfigItem& cfgitem,
        Action action = {}
    ):
        key_{key},
        action_{std::move(action)},
        cfgitem_{std::addressof(cfgitem)}
    {}

    /**
     * @brief Applies a menu option with the given value.
     * @details Sets the configuration item with the given value and invokes the stored
     *     action if not empty.
     * @tparam Value Type of the value.
     * @param[in] value Value to set the configuration item with.
     */
    template<typename Value>
    constexpr auto apply(Value&& value) -> void {
        cfgitem_->set(std::forward<Value>(value));
        if (not action_) return;
        std::invoke(std::forward<Action>(action_));
    }

    /**
     * @brief Returns a string representation of a menu option.
     * @details Contains the name and value of the configuration item.
     * @tparam String Representation type, required to be a std::string.
     * @remark The type-constraint is there for future specializations.
     */
    template<std::same_as<std::string> String>
    [[nodiscard]]
    constexpr auto to() const -> String {
        return std::format("{:20} {:>16}\n",
            cfgitem_->name(), cfgitem_->to<std::string>());
    }

    /**
     * @brief Returns the key of the menu option.
     */
    [[nodiscard]]
    constexpr auto key() const noexcept -> unsigned char
    { return key_; }

    /**
     * @brief Menu options are considered to be equal when their keys match.
     */
    [[nodiscard]]
    friend constexpr auto operator==(
        menu_option const& lhs, menu_option const& rhs) noexcept -> bool
    { return lhs.key() == rhs.key(); }

private:
    Action action_;                        /**< Optional menu action. */
    util::access_ptr<ConfigItem> cfgitem_; /**< Configuration item. */
    unsigned char key_;                    /**< Key to uniquely identify a menu option. */
};

/**
 * @class menu
 * @brief Menu.
 * @details Consists of a set of menu options and holds a menu selection.
 * @tparam ConfigItem Configuration item type, required to be configurable.
 * @tparam Action Action type, required to be invocable without parameters.
 */
template<cc::configurable ConfigItem, std::invocable Action>
class menu {
    using option_type = menu_option<ConfigItem, Action>; /**< Menu option object. */

public:
    /**
     * @brief Adds a menu option to the menu.
     * @post Invalidates the current menu selection when a reallocation is required.
     * @tparam Ts Optional argument types.
     * @param[in] key Key to uniquely identify a menu option.
     * @param[in] args Optional arguments to construct a menu option with.
     */
    template<typename... Ts>
    constexpr auto add(unsigned char key, Ts&&... args) -> void
    { options_.emplace_back(key, std::forward<Ts>(args)...); }

    /**
     * @brief Removes a menu option from the menu.
     * @pre Ensure the current menu selection is valid before calling this function.
     */
    constexpr auto remove() -> void
    { options_.erase(selection); }

    /**
     * @brief Selects a menu option with the given key.
     * @param[in] key Key of the menu option to select.
     * @return If the menu option was found, returns true. Otherwise, returns false.
     */
    constexpr auto select(unsigned char key) noexcept -> bool {
        selection_ = std::ranges::find_if(options_,
            [key](auto const& option){ return option.key() == key; });
        return selection_ != options_.end();
    }

    /**
     * @brief Returns a reference to the selected menu option.
     * @pre Ensure the current menu selection is valid before calling this function.
     */
    [[nodiscard]]
    constexpr auto selection() const noexcept -> option_type&
    { return *selection_; }

    /**
     * @brief Returns a string representation of a menu.
     * @details Contains the string representation of all menu options.
     * @tparam String Representation type, required to be a std::string.
     * @remark The type-constraint is there for future specializations.
     */
    template<std::same_as<std::string> String>
    [[nodiscard]]
    constexpr auto to() const -> String {
        auto result = String{};
        auto const max_option_name = 32;
        result.reserve(max_option_name * options_.size());

        for (auto const& option : options_) {
            result += std::format("{:c} | {}",
                std::toupper(option.key()), option.to<String>());
        }
        return result;
    }

    /**
     * @brief Compares two objects for equality.
     */
    [[nodiscard]]
    friend auto operator==(menu const&, menu const&) -> bool = default;

private:
    using storage_type = std::vector<option_type>; /**< Menu options container type. */
    using selection_type = storage_type::iterator; /**< Menu selection type. */

    storage_type options_;     /**< Menu options. */
    selection_type selection_; /**< Menu selection. */
};

} // namespace ui

#endif
