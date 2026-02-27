#include "pch.h"
#include <filesystem>
#include "Engine/Logging/Log.h"

int main(int argc, char** argv)
{
    // Log::Init() creates a rotating file sink at "../log/app.log" relative to
    // the exe.  Create the directory first so spdlog doesn't throw.
    std::filesystem::create_directories("../log");
    mts::Log::Init();

    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    mts::Log::Shutdown();
    return result;
}
