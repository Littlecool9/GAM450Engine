@echo off

if exist "%userprofile%\OneDrive\Documents\Visual Studio 2022\Templates\ItemTemplates\" (
    xcopy ".\*.zip" "%userprofile%\OneDrive\Documents\Visual Studio 2022\Templates\ItemTemplates\RMEngine\" /Y
) else (
    echo Skipping Visual Studio 2022 templates folder (not found)
)

if exist "%userprofile%\OneDrive\Documents\Visual Studio 18\Templates\ItemTemplates\" (
    xcopy ".\*.zip" "%userprofile%\OneDrive\Documents\Visual Studio 18\Templates\ItemTemplates\RMEngine\" /Y
) else (
    echo Skipping Visual Studio 18 templates folder (not found)
)

pause
