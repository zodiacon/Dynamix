#pragma once

#include <type_traits>

// Forward declaration of a function to enable bitmask operators
template<typename T>
constexpr bool enable_bitmask_operators(T) { return false; } // Default: not enabled

template<typename T>
constexpr std::enable_if_t < std::is_enum_v<T>&& enable_bitmask_operators(T{}), T >
operator|(T lhs, T rhs) {
	using underlying = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>&& enable_bitmask_operators(T{}), T>
operator&(T lhs, T rhs) {
	using underlying = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename T>
constexpr std::enable_if_t < std::is_enum_v<T>&& enable_bitmask_operators(T{}), T >
operator^(T lhs, T rhs) {
	using underlying = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}
