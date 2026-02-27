/**
 * @file ProfilerPanel.h
 * @author sumin.park
 * @brief  Editor panel for FPS and memory profiling
 *
 * @copyright Copyright (c) 2026 - Mitosis Engine
 *
 */

#pragma once
#include <vector>

namespace mts
{
    class ProfilerPanel
    {
       public:
        ProfilerPanel();
        void Draw(float dt);

       private:
        struct RingBuffer
        {
            std::vector<float> data;
            int  head   = 0;
            bool filled = false;

            explicit RingBuffer(int size);
            void Push(float v);
            void CopyChronological(std::vector<float>& out) const;
        };

        RingBuffer         mFpsHist;
        RingBuffer         mMemHistKB;
        std::vector<float> mTmp;

        static float BytesToKB(std::size_t b);
        static float BytesToMB(std::size_t b);
    };

}  // namespace mts
