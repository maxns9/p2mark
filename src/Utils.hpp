/*
* Project: p2mark
* File:    Utils.hpp
* Desc:    Various utility functions header file
* Created: 2025-10-08
*/

#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <Windows.h>
#include <rpcdce.h>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <cassert>

#include "tinyxml2.h"

#include "P2Exception.hpp"

namespace fs = std::filesystem;
using namespace tinyxml2;

namespace p2mark::StringUtils {
    std::vector<std::string> SplitString(std::string_view str, const char delimeter);
    std::string& StringToLower(std::string& str);
    void MakeSingularIfNeeded(std::string& str, const int& count);
}

namespace p2mark::FilesystemUtils {
    bool IsReadOnly(const fs::path& path);
}

namespace p2mark::XmlUtils {
    XMLElement* FindDeepElement(XMLElement* root, std::string_view path);
}

namespace p2mark::WindowsUtils {
    std::string GenerateGuid();
}