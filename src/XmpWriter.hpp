/*
* Project: p2mark
* File:    XmpWriter.hpp
* Desc:    XMP writer class header file
* Created: 2025-10-08
*/

#pragma once

#include <format>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>
#include <cassert>

#include "tinyxml2.h"

#include "Constants.hpp"
#include "Marker.hpp"
#include "P2Exception.hpp"
#include "Utils.hpp"

namespace fs = std::filesystem;
using namespace tinyxml2;

namespace p2mark {
    struct XmlNode {
        std::string name {};
        std::vector<std::pair<std::string, std::string>> attributes {};
    };

    class XmpWriter {
    public:
        explicit XmpWriter(const fs::path& xmpFilePath,
                           std::span<const Marker> markers);

    public:
        void WriteDestinationXmp();

    private:
        void CreateXmpFile();
        void ParseSourceXmp();

        /// Creates a flat list of XML nodes from a list of node names.
        std::vector<XMLElement*> CreateXmpTree(const std::vector<XmlNode>& nodeList);

        /// Connects a flat list of XML nodes sequentially
        /// (the next element will become a child of the current one).
        void ConnectXmpNodes(const std::vector<XMLElement*>& elems);

        /// Introduce the marker data into the XMP.
        void AppendMarkersToXml(XMLElement* markerRoot);

    private:
        /// The base XMP structure.
        static const std::vector<XmlNode> m_XmpBaseStructure;

        /// The marker structure of the XMP.
        static const std::vector<XmlNode> m_XmpMarkerStructure;

    private:
        const fs::path m_FilePath;
        std::span<const Marker> m_Markers;
        tinyxml2::XMLDocument m_XmlDoc;
    };
}