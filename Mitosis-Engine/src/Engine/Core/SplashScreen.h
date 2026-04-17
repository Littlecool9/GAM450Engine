/**
 * @file SplashScreen.h
 * @author sumin.park
 * @brief  Splash screen configuration and runner.
 *         Game projects configure entries via GetSplashScreens() or LoadFromJson().
 *
 * @copyright Copyright (c) 2026 DigiPen (USA) Corporation
 *
 */

#pragma once
#include <string>
#include <vector>

namespace mts
{
    class Window;

    enum class SplashAnimation
    {
        FadeInOut,
    };

    struct SplashScreenConfig
    {
        std::string     texturePath;                              // path to a .tex resource
        float           fadeInDuration  = 0.5f;                  // seconds
        float           holdDuration    = 1.5f;                  // seconds
        float           fadeOutDuration = 0.5f;                  // seconds
        SplashAnimation animation       = SplashAnimation::FadeInOut;
    };

    class SplashScreen
    {
       public:
        // Run the splash sequence. Returns when all entries finish or the window is closed.
        // isRunning is Application::mIsRunning — checked each frame to handle window close.
        static void Run(const std::vector<SplashScreenConfig>& configs,
                        Window&                                window,
                        bool&                                  isRunning);

        // Load splash configs from a JSON file.
        // Format: { "splashScreens": [ { "texturePath", "fadeInDuration",
        //                                "holdDuration", "fadeOutDuration" } ] }
        // Returns an empty vector and logs a warning on failure.
        static std::vector<SplashScreenConfig> LoadFromJson(const std::string& path);
    };

}  // namespace mts
