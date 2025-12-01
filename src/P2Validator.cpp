/*
* Project: p2mark
* File:    P2Validator.cpp
* Desc:    P2 validator class implementation file
* Created: 2025-10-30
*/

#include "P2Validator.hpp"

namespace p2mark {
    P2ValidationResult P2Validator::Validate(const fs::path& contentsDirPath) {
        P2ValidationResult validationResult {};
        const fs::path clipsDirPath {contentsDirPath / CLIP_DIR};

        validationResult = ValidateContentsDir(contentsDirPath);
        if(validationResult != P2ValidationResult::CORRECT_STRUCTURE) {
            return validationResult;
        }

        validationResult = ValidateClipsDir(clipsDirPath);
        if(validationResult != P2ValidationResult::CORRECT_STRUCTURE) {
            return validationResult;
        }

        return P2ValidationResult::CORRECT_STRUCTURE;
    }

    ClipValidationResult P2Validator::ValidateClip(const fs::directory_entry& clipFile) {
        const bool regular {clipFile.is_regular_file()};
        const bool hasRightExt {clipFile.path().extension().string() == XML_EXT};
        const uintmax_t fileSize {clipFile.file_size()};

        if(fileSize == 0) {
            return ClipValidationResult::EMPTY_CLIP_FILE; // Useless empty file
        } else if(fileSize >= P2Validator::CLIP_SIZE_LIMIT) {
            return ClipValidationResult::SUSPICIOUSLY_LARGE_CLIP_FILE; // Potential security risk
        } else if(!hasRightExt) {
            return ClipValidationResult::WRONG_EXT_CLIP_FILE; // Wrong file type
        } else if(!regular) {
            return ClipValidationResult::UNSUPPORTED_CLIP_FILE; // Is a symlink, directory or a device file
        }

        return ClipValidationResult::CORRECT_CLIP_FILE;
    }

    P2ValidationResult P2Validator::ValidateContentsDir(const fs::path& contentsDirPath) {
        const fs::path expectedDir(CONTENTS_DIR);

        if(!fs::exists(contentsDirPath)) {
            return P2ValidationResult::CONTENTS_DIR_MISSING;
        } else if(!fs::is_directory(contentsDirPath)) {
            return P2ValidationResult::CONTENTS_ISNT_DIRECTORY;
        } else if(P2Validator::GetFinalComponent(contentsDirPath) != expectedDir) {
            return P2ValidationResult::NOT_A_CONTENTS_DIR;
        } else if(contentsDirPath.empty()) {
            return P2ValidationResult::CONTENTS_IS_EMPTY;
        }

        return P2ValidationResult::CORRECT_STRUCTURE;
    }

    P2ValidationResult P2Validator::ValidateClipsDir(const fs::path& clipsDirPath) {
        if(!fs::exists(clipsDirPath)) {
            return P2ValidationResult::CLIP_DIR_MISSING;
        } else if(fs::is_empty(clipsDirPath)) {
            return P2ValidationResult::CLIP_DIR_EMPTY;
        }

        return P2ValidationResult::CORRECT_STRUCTURE;
    }

    fs::path P2Validator::GetFinalComponent(const fs::path& path) {
        fs::path normalisedPath {path.lexically_normal()};

        if(normalisedPath.filename().empty()) {
            if(normalisedPath.has_parent_path()) {
                return normalisedPath.parent_path().filename();
            }
            return normalisedPath; // Root directory
        }

        return normalisedPath.filename();
    }
}