/*
* Project: p2mark
* File:    AppInfo.hpp
* Desc:    Contains application metadata
* Created: 2025-10-12
*/

#pragma once

#include <string>
#include <string_view>
#include <format>

namespace p2mark {
    struct AppVersion {
        static inline constexpr unsigned int Major {1U};
        static inline constexpr unsigned int Minor {0U};

        const std::string ToString() const noexcept { return std::format("{}.{}", Major, Minor); }
    };

    struct AppInfo {
        static inline constexpr std::string_view Name        {"p2mark"};
        static inline constexpr std::string_view Description {"A simple utility for converting P2 camera markers so they display properly in Adobe Premiere Pro."};
        static inline constexpr AppVersion       Version     {};
        static inline constexpr std::string_view Author      {"Author: Maxim Chinkin, <Princ_Datski1999 at mail.ru>"};
    };
}