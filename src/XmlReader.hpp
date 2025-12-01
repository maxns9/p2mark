/*
* Project: p2mark
* File:    XmlReader.hpp
* Desc:    XML reader class header file
* Created: 2025-10-07
*/

#pragma once

#include <filesystem>
#include <iostream>
#include <optional>
#include <sstream>
#include <string_view>

#include "tinyxml2.h"

#include "Marker.hpp"
#include "P2Exception.hpp"
#include "Utils.hpp"

namespace fs = std::filesystem;
using namespace tinyxml2;

namespace p2mark {
    class XmlReader {
    public:
        // Per P2 standard, this is max markers per clip
        static inline constexpr size_t MARKERS_VECTOR_RESERVE {100};

        static inline constexpr std::string_view MEMO_LIST_ELEM {"MemoList"};
        static inline constexpr std::string_view MEMO_ELEM      {"Memo"};
        static inline constexpr std::string_view OFFSET_ELEM    {"Offset"};
        static inline constexpr std::string_view TEXT_ELEM      {"Text"};

    public:
        explicit XmlReader(const fs::path& xmlFilePath);

    public:
        std::vector<Marker> ParseSourceXml();

    private:
        /// Parses the TextMemo section of the clip file
        /// and returns a marker structure (if it exists).
        std::optional<Marker> ParseTextMemoElement(XMLElement* elem);

    private:
        const fs::path m_FilePath;
        tinyxml2::XMLDocument m_XmlDoc;
        std::vector<Marker> m_Markers;
    };
}