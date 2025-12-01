#include "ClipProcessor.h"

namespace p2mark {

    ClipProcessor::ClipProcessor(const fs::path& clipDirPath, const fs::path& xmlPath)
        : m_ClipDirPath(clipDirPath), m_XmlPath(xmlPath) {}

    size_t ClipProcessor::ProcessClip(const AppMode mode) {
        const std::string xmlFileName = m_XmlPath.filename().string();
        const std::string xmpFileName = m_XmlPath.stem().string() + XMP_EXT.data();

        std::vector<Marker> markers = XmlReader(m_XmlPath).ParseSourceXml();

        // In this case, nothing else to do
        if(markers.empty()) {
            return 0;
        }

        if(IsProcessMode(mode)) {
            const fs::path xmpFilePath = fs::path(m_ClipDirPath / xmpFileName);
            XmpWriter(xmpFilePath, markers).WriteDestinationXmp();
        }

        return markers.size();
    }
}