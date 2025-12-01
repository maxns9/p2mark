/*
* Project: p2mark
* File:    Utils.cpp
* Desc:    Various utility functions implementation file
* Created: 2025-10-08
*/

#include "Utils.hpp"

namespace p2mark::StringUtils {
    std::vector<std::string> SplitString(std::string_view str, const char delimeter) {
        std::vector<std::string> tokens {};
        std::stringstream ss(str.data());
        std::string token {};

        tokens.reserve(10);

        while(std::getline(ss, token, delimeter)) {
            tokens.emplace_back(token);
        }

        return tokens;
    }

    std::string& StringToLower(std::string& str) {
        for(char& c : str) {
            c = std::tolower(static_cast<unsigned char>(c));
        }

        return str;
    }

    void MakeSingularIfNeeded(std::string& str, const int& count) {
        if(!str.empty() && count == 1) str.pop_back();
    }
}

namespace p2mark::FilesystemUtils {
    bool IsReadOnly(const std::filesystem::path& path) {
        const fs::perms permissions {fs::status(path).permissions()};

        return (permissions & fs::perms::owner_write) == fs::perms::none &&
            (permissions & fs::perms::group_write) == fs::perms::none &&
            (permissions & fs::perms::others_write) == fs::perms::none;
    }
}

namespace p2mark::XmlUtils {
    XMLElement* FindDeepElement(XMLElement* root, std::string_view path) {
        assert(root != nullptr && "FindDeepElement: root must not be null.");
        assert(!path.empty() && "FindDeepElement: path must not be empty.");

        if(root->NoChildren()) {
            return nullptr;
        }

        XMLElement* current {root};
        std::vector<std::string> nodeNames {p2mark::StringUtils::SplitString(path, '/')};

        for(const std::string& n : nodeNames) {
            if(!current) return nullptr;
            current = current->FirstChildElement(n.c_str());
        }

        return current;
    }
}

namespace p2mark::WindowsUtils {
    std::string GenerateGuid() {
        // The GUID should be no more than 40 characters in length
        // Windows adds hyphens and braces: {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
        constexpr size_t GUID_LEN {40};

        GUID guid {};
        if(CoCreateGuid(&guid) != S_OK) {
            throw P2Exception("Can\'t generate a GUID - check your Windows installation",
                              P2ExceptionCode::CODE_GENERIC);
        }

        WCHAR guidStr[GUID_LEN] {};
        if(StringFromGUID2(guid, guidStr, GUID_LEN) == 0) {
            throw P2Exception("Can\'t convert a GUID into string - check your Windows installation",
                              P2ExceptionCode::CODE_GENERIC);
        }

        // Conversion from a wide string to a short string
        size_t narrowLen {static_cast<size_t>(WideCharToMultiByte(CP_UTF8, 0, guidStr, -1, nullptr, 0, nullptr, nullptr))};
        std::string s(narrowLen, 0);
        WideCharToMultiByte(CP_UTF8, 0, guidStr, -1, s.data(), static_cast<int>(narrowLen), nullptr, nullptr);

        s.resize(narrowLen - 1); // Remove the null terminator
        return p2mark::StringUtils::StringToLower(s).substr(1, s.size() - 2); // substr removes the braces
    }
}