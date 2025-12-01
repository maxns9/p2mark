#pragma once

#include <filesystem>
#include <span>

#include "AppMode.h"
#include "XmlReader.h"
#include "XmpWriter.h"

namespace fs = std::filesystem;

namespace p2mark {
    struct Marker;

    class ClipProcessor {
    public:
        explicit ClipProcessor(const fs::path& clipDirPath, const fs::path& xmlPath);

    public:
        /// Read P2 clip XML files, and write them to Adobe's XMP
        /// files if the correct application mode is set.
        size_t ProcessClip(const AppMode mode);

    private:
        const fs::path m_ClipDirPath;
        const fs::path m_XmlPath;
    };
}