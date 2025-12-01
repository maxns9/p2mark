/*
* Project: p2mark
* File:    P2Exception.hpp
* Desc:    Custom P2 exception
* Created: 2025-10-07
*/

#pragma once

#include <exception>
#include <string>

namespace p2mark {
    /// Specifies the kind of exception
    /// without the need for sub-classes.
    enum class P2ExceptionCode {
        CODE_GENERIC = 0,
        CODE_FILESYSTEM_ERROR,
        CODE_XML_READ_ERROR,
        CODE_XMP_READ_ERROR,
        CODE_XMP_WRITE_ERROR
    };

    class P2Exception : public std::exception {
    public:
        P2Exception(const std::string& message)
            : m_Message(message), m_Code(P2ExceptionCode::CODE_GENERIC) {}
        P2Exception(const std::string& message, const P2ExceptionCode code) :
            m_Message(message), m_Code(code) {}

        const char* what() const noexcept override { return m_Message.c_str(); }
        const P2ExceptionCode code() const noexcept { return m_Code; }

    private:
        const std::string m_Message;
        const P2ExceptionCode m_Code;
    };
}