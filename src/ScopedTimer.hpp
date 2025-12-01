/*
* Project: p2mark
* File:    ScopedTimer.hpp
* Desc:    Simple scoped code timer
* Created: 2025-11-04
*/

#pragma once

#ifdef _DEBUG
#define SCOPED_TIMER ScopedTimer t
#else
#define SCOPED_TIMER
#endif

#include <iostream>
#include <chrono>

namespace p2mark {
    /// A simple scoped timer taken from the Cherno's benchmarking video.
    class ScopedTimer {
    public:
        ScopedTimer() {
            m_StartTime = std::chrono::high_resolution_clock::now();
        }

        ~ScopedTimer() {
            m_EndTime = std::chrono::high_resolution_clock::now();

            long long startPoint {std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTime).time_since_epoch().count()};
            long long endPoint   {std::chrono::time_point_cast<std::chrono::microseconds>(m_EndTime).time_since_epoch().count()};
            double    duration   {(endPoint - startPoint) * 0.001};

            std::cout << std::format("Processing took: {} ms.\n", duration);
        }

    private:
        std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime {};
        std::chrono::time_point<std::chrono::high_resolution_clock> m_EndTime   {};
    };
}