/*
* Project: p2mark
* File:    Marker.hpp
* Desc:    P2 TextMemo (or 'Marker') structure
* Created: 2025-10-08
*/

#pragma once

#include <string>

namespace p2mark {
    /// A P2 marker. The actual marker data has an additional
    /// 'MemoID' attribute, but it serves no purpose for us;
    /// this is for addressing markers in the camcorder.
    struct Marker {
        int offset       {};
        std::string text {};
    };
}