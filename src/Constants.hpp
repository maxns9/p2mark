/*
* Project: p2mark
* File:    Constants.hpp
* Desc:    General application-wide constant values
* Created: 2025-11-02
*/

#pragma once

#include <string_view>

namespace p2mark {
    // P2 structure
    inline constexpr std::string_view CONTENTS_DIR {"CONTENTS"};
    inline constexpr std::string_view CLIP_DIR     {"CLIP"};

    // File extensions
    inline constexpr std::string_view XML_EXT {".XML"};
    inline constexpr std::string_view XMP_EXT {".XMP"};
}