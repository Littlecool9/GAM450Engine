/**
 * @file FPSCounter.h
 * @author rahul
 * @brief FPS Counter for the Engine
 *
 * @copyright Copyright (c) 2025 - Mitosis Engine
 *
 */

#pragma once

namespace mts
{
    class FPSCounter
    {
       public:
        // Returns true when a new 1-second sample is computed (log now)
        bool Update(double frameDtSeconds)
        {
            m_frames++;
            m_elapsed += frameDtSeconds;

            if (m_elapsed >= 1.0)
            {
                m_fps = m_frames / m_elapsed;
                m_ms = (m_fps > 0.0) ? (1000.0 / m_fps) : 0.0;

                m_frames = 0;
                m_elapsed = 0.0;
                return true;
            }

            return false;
        }

        int FPSRounded() const { return static_cast<int>(std::lround(m_fps)); }
        double FPS() const { return m_fps; }
        double FrameMS() const { return m_ms; }

       private:
        int m_frames = 0;
        double m_elapsed = 0.0;
        double m_fps = 0.0;
        double m_ms = 0.0;
    };
}  // namespace mts
