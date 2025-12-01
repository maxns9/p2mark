/*
* Project: p2mark
* File:    XmpWriter.cpp
* Desc:    XMP writer class implementation file
* Created: 2025-10-08
*/

#include "XmpWriter.hpp"

namespace p2mark {
    const std::vector<XmlNode> XmpWriter::m_XmpBaseStructure {
        {"x:xmpmeta", {
            {"xmlns:x", "adobe:ns:meta/"},
            {"x:xmptk", "Adobe XMP Core 7.0-c000 79.1357c9e, 2021/07/14-00:39:56        "}
        }},
        {"rdf:RDF", {
            {"xmlns:rdf", "http://www.w3.org/1999/02/22-rdf-syntax-ns#"}
        }},
        {"rdf:Description", {
            {"rdf:about",   ""},
            {"xmlns:dc",    "http://purl.org/dc/elements/1.1/"},
            {"xmlns:xmpDM", "http://ns.adobe.com/xmp/1.0/DynamicMedia/"},
            {"xmlns:stDim", "http://ns.adobe.com/xap/1.0/sType/Dimensions#"},
            {"xmlns:xmp",   "http://ns.adobe.com/xap/1.0/"},
            {"xmlns:tiff",  "http://ns.adobe.com/tiff/1.0/"},
            {"xmlns:aux",   "http://ns.adobe.com/exif/1.0/aux/"},
            {"xmlns:xmpMM", "http://ns.adobe.com/xap/1.0/mm/"},
            {"xmlns:stEvt", "http://ns.adobe.com/xap/1.0/sType/ResourceEvent#"},
        }},
        {"xmpDM:Tracks", {}},
        {"rdf:Bag", {}},
        {"rdf:li", {}},
        {"rdf:Description", {
            {"xmpDM:trackName", "Comment"},
            {"xmpDM:trackType", "Comment"},
            {"xmpDM:frameRate", "f25"}
        }},
        {"xmpDM:markers", {}},
        {"rdf:Seq", {}}
    };

    const std::vector<XmlNode> XmpWriter::m_XmpMarkerStructure {
        {"rdf:li", {}},
        {"rdf:Description", {
            {"xmpDM:startTime", ""},
            {"xmpDM:guid", ""}
        }},
        {"xmpDM:cuePointParams", {}},
        {"rdf:Seq", {}},
        {"rdf:li", {
            {"xmpDM:key", "marker_guid"},
            {"xmpDM:value", ""}
        }}
    };

    XmpWriter::XmpWriter(const fs::path& xmpFilePath, std::span<const Marker> markers) :
        m_FilePath(xmpFilePath), m_Markers(markers) {}

    void XmpWriter::WriteDestinationXmp() {
        if(!fs::exists(m_FilePath)) {
            CreateXmpFile();
        } else {
            ParseSourceXmp();
        }
    }

    // XMP's structure is EXTREMELY SHIT
    // read this with your eyes closed
    void XmpWriter::CreateXmpFile() {
        // Create the basic XMP tree
        std::vector<XMLElement*> elems {CreateXmpTree(XmpWriter::m_XmpBaseStructure)};
        ConnectXmpNodes(elems);
        m_XmlDoc.InsertFirstChild(elems[0]);

        const size_t last {elems.size() - 1};
        AppendMarkersToXml(elems[last]);

        const bool compactXml {false};
        if(m_XmlDoc.SaveFile(m_FilePath.string().c_str(), compactXml) != XML_SUCCESS) {
            throw P2Exception(std::format("Can't save {}", m_FilePath.filename().string()),
                              P2ExceptionCode::CODE_XMP_WRITE_ERROR);
        }
    }

    void XmpWriter::ParseSourceXmp() {
        if(m_XmlDoc.LoadFile(m_FilePath.string().c_str()) != XML_SUCCESS) {
            throw P2Exception("Can\'t load XMP file",
                              P2ExceptionCode::CODE_XMP_READ_ERROR);
        }

        XMLElement* root = m_XmlDoc.RootElement();
        if(!root) {
            throw P2Exception("The XMP file is damaged or has incorrect type",
                              P2ExceptionCode::CODE_XMP_READ_ERROR);
        }

        constexpr std::string_view markerListPath {"rdf:RDF/rdf:Description/xmpDM:Tracks/rdf:Bag/rdf:li/rdf:Description/xmpDM:markers/rdf:Seq"};
        XMLElement* markerListElem {p2mark::XmlUtils::FindDeepElement(root, markerListPath)};

        if(!markerListElem) {
            throw P2Exception("Can\'t load XMP file",
                              P2ExceptionCode::CODE_XMP_READ_ERROR);
        }

        // If the file is read-only, we can't write markers into it
        if(p2mark::FilesystemUtils::IsReadOnly(m_FilePath)) {
            throw P2Exception("XMP file is marked as read-only",
                              P2ExceptionCode::CODE_XMP_WRITE_ERROR);
        }

        // If there are already markers inside, don't do anything to this file;
        // these could be important editor's markers
        if(!markerListElem->NoChildren()) {
            throw P2Exception("XMP file already contains markers",
                              P2ExceptionCode::CODE_XMP_WRITE_ERROR);
        }

        AppendMarkersToXml(markerListElem);
        if(m_XmlDoc.SaveFile(m_FilePath.string().c_str(), false) != XML_SUCCESS) {
            throw P2Exception(std::format("Can't save {}", m_FilePath.filename().string()),
                              P2ExceptionCode::CODE_XMP_WRITE_ERROR);
        }
    }

    std::vector<XMLElement*> XmpWriter::CreateXmpTree(const std::vector<XmlNode>& nodeList) {
        // Creates a single XMP node with the correct name and attributes
        auto createSingleNode = [this](const XmlNode& node) -> XMLElement* {
            XMLElement* xmlElem {m_XmlDoc.NewElement(node.name.c_str())};

            for(const auto& [attrName, attrVal] : node.attributes) {
                xmlElem->SetAttribute(attrName.c_str(), attrVal.c_str());
            }

            return xmlElem;
        };

        std::vector<XMLElement*> elems {};
        elems.reserve(nodeList.size());

        for(const auto& node : nodeList) {
            elems.emplace_back(createSingleNode(node));
        }

        return elems;
    }

    void XmpWriter::ConnectXmpNodes(const std::vector<XMLElement*>& elems) {
        for(size_t i {0}; i < elems.size() - 1; i++) {
            XMLElement* current {elems[i]};
            XMLElement* next    {elems[i + 1]};

            if(!current || !next) continue;
            current->InsertFirstChild(next);
        }
    }

    void XmpWriter::AppendMarkersToXml(XMLElement* markerRoot) {
        if(!markerRoot)
            return;

        constexpr auto insertFrameOffset = [](XMLElement* descriptionElem, int offset) -> void {
            descriptionElem->SetAttribute("xmpDM:startTime", offset);
        };

        constexpr auto insertGuid = [](XMLElement* descriptionElem, XMLElement* liElem, std::string_view guid) -> void {
            descriptionElem->SetAttribute("xmpDM:guid", guid.data());
            liElem->SetAttribute("xmpDM:value", guid.data());
        };

        constexpr auto insertMarkerText = [](XMLElement* descriptionElem, std::string_view text) -> void {
            descriptionElem->SetAttribute("xmpDM:name", text.data());
        };

        for(const Marker& mark : m_Markers) {
            std::vector<XMLElement*> markerElems {CreateXmpTree(XmpWriter::m_XmpMarkerStructure)};
            ConnectXmpNodes(markerElems);

            XMLElement* descriptionElem {markerElems[1]}; // rdf:Description
            XMLElement* liElem          {markerElems[4]}; // rdf:Description/xmpDM:cuePointParams/rdf:Seq/rdf:li

            insertFrameOffset(descriptionElem, mark.offset);
            insertGuid(descriptionElem, liElem, p2mark::WindowsUtils::GenerateGuid());
            if(!mark.text.empty()) insertMarkerText(descriptionElem, mark.text);

            markerRoot->InsertEndChild(markerElems[0]);
        }
    }
}