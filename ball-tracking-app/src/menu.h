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
#include <functional>
#include <string>
#include <type_traits>
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
 */
template<typename Accessor, typename Mutator>
class dispatcher {
public:
    /**
     * @brief ..
     */
    dispatcher(Accessor accessor, Mutator mutator):
        accessor_{std::move(accessor)},
        mutator_{std::move(mutator)}
    {}

    /**
     * @brief ..
     * @details ..
     * @return ..
     */
    constexpr auto accessor() const noexcept -> Accessor const&
    { return accessor_; }

    /**
     * @brief ..
     * @details ..
     * @return ..
     */
    constexpr auto mutator() const noexcept -> Mutator const&
    { return mutator_; }

    /**
     * @brief ..
     */
    friend auto operator==(dispatcher const&, dispatcher const&) -> bool = default;

private:
    Accessor accessor_;
    Mutator mutator_;
};

/**
 * @struct ..
 * @brief ..
 * @details ..
 * @{
 */
template<typename...>
struct class_type_of;

template<typename Signature, typename Class>
struct class_type_of<Signature Class::*> {
    using class_type = Class;
};
/** @} */

/**
 * @typedef ..
 * @brief ..
 */
template<typename F>
using class_type_of_t = typename class_type_of<F>::class_type;

/**
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 * @tparam ..
 * @return ..
 * 
 * @todo require non-types to be pointer to member functions
 */
template<auto Accessor, auto Mutator, typename Value = void>
constexpr auto dispatch() noexcept {
    using accessor_type = decltype(Accessor);
    using class_type = class_type_of_t<accessor_type>;
    using result_type = std::invoke_result_t<accessor_type, class_type>;
    using value_type = std::conditional_t<
        std::is_same_v<Value, void>, result_type, Value>;
    return dispatcher{
        +[](class_type const& object) -> value_type
        { return std::invoke(Accessor, object); },
        +[](class_type& object, value_type value)
        { std::invoke(Mutator, object, value); }
    };
}

/**
 * @class ..
 * @brief ..
 * @details ..
 * @tparam ..
 * @tparam ..
 * 
 * @todo require types to be invocable
 */
template<typename Accessor, typename Mutator>
class option {
    /**
     * @typedef ..
     * @brief ..
     */
    using dispatcher_type = dispatcher<Accessor, Mutator>;

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
    constexpr option(std::string name, dispatcher_type dispatcher_obj):
        name_{std::move(name)},
        dispatcher_{std::move(dispatcher_obj)}
    {}

    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @return ..
     */
    template<typename... Ts>
    constexpr auto getvalue(Ts&&... args) const noexcept -> decltype(auto)
    { return std::invoke(dispatcher_.accessor(), std::forward<Ts>(args)...); }

    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @return ..
     */
    template<typename... Ts>
    constexpr auto setvalue(Ts&&... args) const noexcept -> void
    { return std::invoke(dispatcher_.mutator(), std::forward<Ts>(args)...); }

    /**
     * @brief ..
     * @details ..
     * @tparam ..
     * @param[in] .. ..
     * @return ..
     */
    template<typename... Ts>
    constexpr auto to_string(Ts&&... args) const -> std::string {
        return std::format("{:16} {:3}\n",
            name_ + ':', getvalue(std::forward<Ts>(args)...));
    }

    /**
     * @brief ..
     */
    friend auto operator==(option const&, option const&) -> bool = default;

private:
    std::string name_;           /**< .. */
    dispatcher_type dispatcher_; /**< .. */
};

/**
 * @class ..
 * @brief ..
 * @details ..
 * @tparam ..
 * 
 * @todo require types to be invocable
 */
template<typename... Dispatchers>
class menu {
    /**
     * @typedef ..
     * @brief ..
     */
    using option_type = option<Dispatchers...>;

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
    constexpr auto selection() const -> option_type const&
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
        auto const max_option_name = 24;
        result.reserve(max_option_name * options.size());

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
    /**
     * @typedef ..
     * @brief ..
     */
    using map_type = std::unordered_map<unsigned char, option_type>;

    map_type options;           /**< .. */
    unsigned char selection_{}; /**< .. */
};

} // namespace opt

#endif
