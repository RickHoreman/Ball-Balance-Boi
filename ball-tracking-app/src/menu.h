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

#ifndef BBB_OPT_MENU_H
#define BBB_OPT_MENU_H

#include <cctype>
#include <format>
#include <map>
#include <string>
#include <utility>

namespace opt {

/**
 * @class ..
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 * 
 * @todo require types to be invocable
 */
template<typename Getter, typename Setter>
class option {
public:
    /**
     * @brief ..
     */
    option() = default;

    /**
     * @brief ..
     * @details ..
     * @param[in] .. ..
     * @param[in] .. ..
     * @param[in] .. ..
     */
    constexpr option(std::string name, Getter getter, Setter setter):
        name_{std::move(name)},
        getter_{std::move(getter)},
        setter_{std::move(setter)}
    {}

    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @return ..
     */
    template<typename... Ts>
    constexpr decltype(auto) getvalue(Ts&&... args) const noexcept
    { return getter_(std::forward<Ts>(args)...); }

    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @return ..
     */
    template<typename... Ts>
    constexpr auto setvalue(Ts&&... args) const noexcept -> void
    { setter_(std::forward<Ts>(args)...); }

    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @return ..
     */
    template<typename... Ts>
    constexpr auto to_string(Ts&&... args) const -> std::string {
        return std::format("{:16}{:3}\n",
            name_ + ':', getvalue(std::forward<Ts>(args)...));
    }

    /**
     * @brief ..
     */
    friend auto operator==(option const&, option const&) -> bool = default;

private:
    std::string name_; /**< .. */
    Getter getter_;    /**< .. */
    Setter setter_;    /**< .. */
};

/**
 * @class ..
 * @brief ..
 * @details ..
 * 
 * @todo require types to be invocable
 */
template<typename... Actions>
class menu {
public:
    /**
     * @brief ..
     * @details ..
     * @param[in] .. ..
     * @return ..
     */
    constexpr auto contains(unsigned char key) const noexcept -> bool
    { return options.contains(key); }

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
    constexpr auto selection() const -> option<Actions...> const&
    { return options.at(selection_); }

    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @param[in] .. ..
     */
    template<typename... Ts>
    constexpr auto add(unsigned char key, Ts&&... args) -> void
    { options.try_emplace(key, std::forward<Ts>(args)...); }

    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @return ..
     */
    template<typename... Ts>
    constexpr auto to_string(Ts&&... args) const -> std::string {
        auto result = std::string{};
        result.reserve(32 * options.size());
        for (auto const& [key, option] : options) {
            auto const upperkey = static_cast<char>(std::toupper(key));
            result += std::format("{} - {}", upperkey,
                option.to_string(std::forward<Ts>(args)...));
        }
        return result;
    }

    /**
     * @brief ..
     */
    friend auto operator==(menu const&, menu const&) -> bool = default;

private:
    std::map<unsigned char, option<Actions...>> options; /**< .. */
    unsigned char selection_{};                          /**< .. */
};

} // namespace opt

#endif
