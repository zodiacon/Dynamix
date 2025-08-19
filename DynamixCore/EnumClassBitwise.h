#pragma once

#include <type_traits>

template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T>
operator|(T lhs, T rhs) {
	using underlying = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<underlying>(lhs) | static_cast<underlying>(rhs));
}

template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T>
operator|=(T& lhs, T rhs) {
	using underlying = std::underlying_type_t<T>;
	return static_cast<T&>(static_cast<underlying>(lhs) |= static_cast<underlying>(rhs));
}

template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T>
operator&(T lhs, T rhs) {
	using underlying = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<underlying>(lhs) & static_cast<underlying>(rhs));
}

template<typename T>
constexpr std::enable_if_t<std::is_enum_v<T>, T>
operator^(T lhs, T rhs) {
	using underlying = std::underlying_type_t<T>;
	return static_cast<T>(static_cast<underlying>(lhs) ^ static_cast<underlying>(rhs));
}
