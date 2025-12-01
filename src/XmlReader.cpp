/*
* Project: p2mark
* File:    XmlReader.cpp
* Desc:    XML reader class implementation file
* Created: 2025-10-07
*/

#include "XmlReader.hpp"

namespace p2mark {
    XmlReader::XmlReader(const fs::path& xmlFilePath) :
        m_FilePath(xmlFilePath) {

        if(m_XmlDoc.LoadFile(xmlFilePath.string().c_str()) != XML_SUCCESS) {
            throw P2Exception("Can\'t load a clip file", P2ExceptionCode::CODE_XML_READ_ERROR);
        }

        m_Markers.reserve(XmlReader::MARKERS_VECTOR_RESERVE);
    }

    // The path is: P2Main -> ClipContent -> ClipMetadata -> MemoList -> Memo
    std::vector<Marker> XmlReader::ParseSourceXml() {
        XMLElement* root {m_XmlDoc.RootElement()};
        if(!root) {
            throw P2Exception("The clip file is damaged or has incorrect type",
                              P2ExceptionCode::CODE_XML_READ_ERROR);
        }

        XMLElement* memoListElem {p2mark::XmlUtils::FindDeepElement(root, "ClipContent/ClipMetadata/MemoList")};

        auto elementIsValid = [](XMLElement*& elem) {
            return (elem != nullptr && !elem->NoChildren());
        };

        // No child elements -> no tags to parse
        if(!elementIsValid(memoListElem)) {
            return m_Markers;
        }

        for(XMLElement* memoElem {memoListElem->FirstChildElement()};
            elementIsValid(memoElem);
            memoElem = memoElem->NextSiblingElement()) {

            std::optional<Marker> mark {ParseTextMemoElement(memoElem)};
            if(mark) {
                m_Markers.emplace_back(*mark);
            }
        }

        return m_Markers;
    }

    std::optional<Marker> XmlReader::ParseTextMemoElement(XMLElement* elem) {
        if(!elem) {
            return std::nullopt;
        }

        Marker mark {};
        const XMLElement* offsetElem {elem->FirstChildElement(XmlReader::OFFSET_ELEM.data())};
        const XMLElement* textElem   {elem->FirstChildElement(XmlReader::TEXT_ELEM.data())};

        if(!offsetElem || offsetElem->QueryIntText(&mark.offset) != XML_SUCCESS) {
            return std::nullopt;
        }

        // Optional node
        if(textElem && textElem->GetText() != nullptr) {
            mark.text = std::string(textElem->GetText());
        }

        return mark;
    }
}