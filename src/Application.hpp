/*
* Project: p2mark
* File:    Application.hpp
* Desc:    Main application class header file
* Created: 2025-10-07
*/

#pragma once

#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "AppInfo.hpp"
#include "AppMode.hpp"
#include "ComGuard.hpp"
#include "Constants.hpp"
#include "P2Exception.hpp"
#include "P2Validator.hpp"
#include "XmlReader.hpp"
#include "XmpWriter.hpp"

namespace fs = std::filesystem;

namespace p2mark {
    struct Marker;

    struct AppStats {
        int ClipsFound       {0};
        int ClipsWithMarkers {0};
        int TotalMarkers     {0};
        int XmlReadErrors    {0};
        int XmpWriteErrors   {0};

        inline bool AreThereMarkers()   const { return ClipsWithMarkers != 0; }
        inline bool AnyXmlReadErrors()  const { return XmlReadErrors != 0; }
        inline bool AnyXmpWriteErrors() const { return XmpWriteErrors != 0; }
    };

    class Application {
    public:
        /// This is how much memory will be allocated for the clip paths vector.
        /// I've analysed about 70 shoots and came to the conclusion that the
        /// average amount of clips in these shoots was around 102.
        /// So this is a generous overestimation.
        static inline constexpr size_t CLIP_FILES_VECTOR_RESERVE {250};

    private:
        static inline constexpr size_t YIELD_AFTER {5};

    public:
        explicit Application(const AppMode mode, std::string_view contentsDirPath);

    public:
        /// Iterates through the CLIP directory
        /// and builds a list of valid clips.
        void RetrieveClipFiles();

        /// Sort clip paths alphabetically because they appear
        /// out of order when printed.
        void SortClipFiles();

        /// Parses clips, retrieves a list of markers from them,
        /// and then, depending on the application mode, either writes
        /// these markers to *.XMP files, or just outputs them as a list.
        void BatchProcessClips();

    private:
        size_t ProcessSingleClip(const fs::path& clipPath, std::string_view xmpFileName);

        /// Print the result for one processed file.
        void PrintFileResult(std::string_view xmlName,
                             std::string_view xmpName,
                             const size_t markerCount) const;

        /// Prints the final output.
        void PrintStats() const;

    private:
        const AppMode m_AppMode;
        const ComGuard m_ComGuard;
        AppStats m_AppStats;

        fs::path m_ContentsDir;
        fs::path m_ClipDir;

        std::vector<fs::path> m_Clips;
    };
}