/*
* Project: p2mark
* File:    AppMode.hpp
* Desc:    Describes which working modes application supports
* Created: 2025-10-30
*/

#pragma once

namespace p2mark {
    /// Parse clips and write XMPs to disk
    /// or just simply read info and display.
    enum class AppMode {
        MODE_LIST_MARKERS = 0,
        MODE_WRITE_MARKERS
    };

    constexpr inline bool IsWriteMode(const AppMode mode) {
        return mode == AppMode::MODE_WRITE_MARKERS;
    }

    constexpr inline std::string_view AppModeToString(const AppMode mode) {
        if(mode == AppMode::MODE_WRITE_MARKERS) {
            return "write";
        } else {
            return "list";
        }
    }
}