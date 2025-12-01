/*
* Project: p2mark
* File:    P2Validator.hpp
* Desc:    P2 validator class header file
* Created: 2025-10-30
*/

#pragma once

#include <filesystem>

#include "Constants.hpp"

namespace fs = std::filesystem;

namespace p2mark {
    enum class P2ValidationResult {
        CORRECT_STRUCTURE = 0,
        CONTENTS_DIR_MISSING,
        CONTENTS_ISNT_DIRECTORY,
        NOT_A_CONTENTS_DIR,
        CONTENTS_IS_EMPTY,
        CLIP_DIR_MISSING,
        CLIP_DIR_EMPTY
    };

    enum class ClipValidationResult {
        CORRECT_CLIP_FILE = 0,
        EMPTY_CLIP_FILE,
        SUSPICIOUSLY_LARGE_CLIP_FILE,
        WRONG_EXT_CLIP_FILE,
        UNSUPPORTED_CLIP_FILE
    };

    /// Validates the P2 structure and individual clip files;
    /// For the purposes of our application, we don't need to validate
    /// the entire P2 structure, only the CLIP directory part.
    class P2Validator {
    public:
        static inline constexpr uintmax_t CLIP_SIZE_LIMIT_MB {2};

    private:
        static inline constexpr uintmax_t CLIP_SIZE_LIMIT {CLIP_SIZE_LIMIT_MB * 1024 * 1024};

    public:
        static P2ValidationResult Validate(const fs::path& contentsDirPath);
        static ClipValidationResult ValidateClip(const fs::directory_entry& clipFile);

    private:
        static P2ValidationResult ValidateContentsDir(const fs::path& contentsDirPath);
        static P2ValidationResult ValidateClipsDir(const fs::path& clipsDirPath);
        static fs::path GetFinalComponent(const fs::path& path);
    };
}