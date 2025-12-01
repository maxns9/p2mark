/*
* Project: p2mark
* File:    Application.cpp
* Desc:    Main application class implementation file
* Created: 2025-10-07
*/

#include "Application.hpp"

namespace p2mark {
    Application::Application(const AppMode mode, std::string_view contentsDirPath) :
    m_AppMode(mode),
    m_ComGuard(),
    m_AppStats(),
    m_ContentsDir(contentsDirPath),
    m_ClipDir(m_ContentsDir / CLIP_DIR) {
        auto result {P2Validator::Validate(m_ContentsDir)};

        if(result == P2ValidationResult::CONTENTS_DIR_MISSING ||
           result == P2ValidationResult::CONTENTS_ISNT_DIRECTORY ||
           result == P2ValidationResult::CONTENTS_IS_EMPTY) {
            throw P2Exception(std::format("The provided path to the {} directory is invalid", CONTENTS_DIR),
                              P2ExceptionCode::CODE_FILESYSTEM_ERROR);
        } else if(result == P2ValidationResult::NOT_A_CONTENTS_DIR) {
            throw P2Exception(std::format(
                "Provided folder must be named {}, without a trailing slash at the end",
                CONTENTS_DIR),
                P2ExceptionCode::CODE_FILESYSTEM_ERROR);
        } else if(result == P2ValidationResult::CLIP_DIR_MISSING) {
            throw P2Exception(std::format(
                "The {} directory is missing. The P2 structure is damaged",
                CLIP_DIR),
                P2ExceptionCode::CODE_FILESYSTEM_ERROR);
        } else if(result == P2ValidationResult::CLIP_DIR_EMPTY) {
            throw P2Exception(std::format("The {} directory is empty", CLIP_DIR),
                              P2ExceptionCode::CODE_FILESYSTEM_ERROR);
        }

        m_Clips.reserve(Application::CLIP_FILES_VECTOR_RESERVE);
    }

    void Application::RetrieveClipFiles() {
        for(const auto& file : fs::directory_iterator(m_ClipDir)) {
            auto result {P2Validator::ValidateClip(file)};

            if(result == ClipValidationResult::CORRECT_CLIP_FILE) {
                m_Clips.emplace_back(file.path());
            } else if(result == ClipValidationResult::SUSPICIOUSLY_LARGE_CLIP_FILE) {
                std::cout << std::format("{} is skipped because it is too large (more than {} MB).\n",
                                   file.path().filename().string(),
                                   P2Validator::CLIP_SIZE_LIMIT_MB);
            }
        }
    }

    void Application::SortClipFiles() {
        std::sort(m_Clips.begin(), m_Clips.end(), [](const fs::path& a, const fs::path& b) {
            return a.filename().string() < b.filename().string();
        });
    }

    void Application::BatchProcessClips() {
        const size_t& clipsCount {m_Clips.size()};
        size_t markerCount {0};

        if(m_Clips.empty()) {
            std::cerr << "No clips found.\n";
            return;
        }

        m_AppStats.ClipsFound = static_cast<int>(clipsCount);

        for(size_t i {0}; i < clipsCount; i++) {
            const fs::path& clip {m_Clips[i]};
            const std::string xmlFileName {clip.filename().string()};
            const std::string xmpFileName {clip.stem().string() + XMP_EXT.data()};

            try {
                markerCount = ProcessSingleClip(clip, xmpFileName);

                // Don't print files without markers in them (clutters standard output)
                if(markerCount > 0) {
                    PrintFileResult(xmlFileName, xmpFileName, markerCount);
                }
            } catch(const P2Exception& e) {
                const P2ExceptionCode& code {e.code()};
                std::string_view msg {e.what()};

                if(IsWriteMode(m_AppMode)) {
                    std::cerr << std::format("{} -> <-------->: {}.\n", xmlFileName, msg);
                } else {
                    std::cerr << std::format("{}: {}.\n", xmlFileName, msg);
                }

                if(code == P2ExceptionCode::CODE_XML_READ_ERROR) {
                    m_AppStats.XmlReadErrors++;
                } else if(code == P2ExceptionCode::CODE_XMP_WRITE_ERROR) {
                    m_AppStats.XmpWriteErrors++;
                }
            } catch(const std::filesystem::filesystem_error& e) {
                const std::string errorDesc {e.what()};

                std::cerr << std::format("Cannot write {}: {}.\n",
                                         xmpFileName, errorDesc);
                return;
            } catch(const std::bad_alloc&) {
                std::cerr << "System is out of memory.\n";
                return;
            }

            // Signal to the OS that it can trigger a context switch
            if(i % YIELD_AFTER == 0) {
                std::this_thread::yield();
            }
        }

        PrintStats();
    }

    size_t Application::ProcessSingleClip(const fs::path& xmlPath, std::string_view xmpFileName) {
        std::vector<Marker> markers {XmlReader(xmlPath).ParseSourceXml()};

        if(markers.empty()) {
            return 0;
        }

        m_AppStats.ClipsWithMarkers++;
        m_AppStats.TotalMarkers += static_cast<int>(markers.size());

        if(IsWriteMode(m_AppMode)) {
            const fs::path xmpFilePath {fs::path(m_ClipDir / xmpFileName)};
            XmpWriter(xmpFilePath, markers).WriteDestinationXmp();
        }

        return markers.size();
    }

    void Application::PrintFileResult(std::string_view xmlName,
                                      std::string_view xmpName,
                                      const size_t markerCount) const {
        std::string markerNoun {"markers"};
        p2mark::StringUtils::MakeSingularIfNeeded(markerNoun, static_cast<int>(markerCount));

        if(IsWriteMode(m_AppMode)) {
            std::cout << std::format("{} -> {}: {} {} written.\n",
                                     xmlName, xmpName, markerCount, markerNoun);
        } else {
            std::cout << std::format("{}: has {} {}.\n",
                                     xmlName, markerCount, markerNoun);
        }
    }

    void Application::PrintStats() const {
        std::stringstream ss {};

        if(m_AppStats.AreThereMarkers()) ss << "\n";
        ss << "Clips in the shoot: " << m_AppStats.ClipsFound << "\n";

        if(m_AppStats.AreThereMarkers()) {
            ss << "Clips with markers: " << m_AppStats.ClipsWithMarkers << "\n";
            ss << "Total number of markers: " << m_AppStats.TotalMarkers << "\n";

            if(m_AppStats.AnyXmlReadErrors()) {
                ss << "XML read errors: " << m_AppStats.XmlReadErrors << "\n";
            }

            if(m_AppStats.AnyXmpWriteErrors()) {
                ss << "XMP write errors: " << m_AppStats.XmpWriteErrors << "\n";
            }
        } else {
            if(IsWriteMode(m_AppMode)) {
                ss << "No markers were written.\n";
            } else {
                ss << "No markers were found.\n";
            }
        }

        std::cout << ss.str();
    }
}