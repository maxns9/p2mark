/*
* Project: p2mark
* File:    ComGuard.hpp
* Desc:    Windows COM wrapper
* Created: 2025-11-01
*/

#pragma once

#include "Utils.hpp"

namespace p2mark {
    /// A very simple RAII wrapper object for Windows' COM.
    class ComGuard {
    public:
        ComGuard() {
            HRESULT hr {CoInitializeEx(nullptr, COINIT_MULTITHREADED)};
            if(FAILED(hr) && hr != RPC_E_CHANGED_MODE) {
                throw P2Exception("Can\'t initialise COM", P2ExceptionCode::CODE_GENERIC);
            }

            // Let's try to generate a GUID to see if there are
            // any issues with COM on this Windows machine;
            // the return value is discarded intentionally
            p2mark::WindowsUtils::GenerateGuid();

            m_ComInitialised = SUCCEEDED(hr);
        }

        ~ComGuard() {
            if(m_ComInitialised) {
                CoUninitialize();
            }
        }

    private:
        bool m_ComInitialised {false};
    };
}