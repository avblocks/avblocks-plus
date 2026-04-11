#pragma once

// C++23 shims for C++20 builds.
// When compiled as C++23 the standard library provides these directly.
#if __cplusplus < 202302L

#include <format>
#include <cstdio>

namespace std {

template<typename... Args>
inline void println(std::FILE* f, std::format_string<Args...> fmt, Args&&... args) {
    std::fputs((std::format(fmt, std::forward<Args>(args)...) + '\n').c_str(), f);
}

template<typename... Args>
inline void println(std::format_string<Args...> fmt, Args&&... args) {
    std::println(stdout, fmt, std::forward<Args>(args)...);
}

} // namespace std

#endif // __cplusplus < 202302L
