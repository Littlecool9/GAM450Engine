# RM-Engine

> **Note:** This project uses the Visual Studio 2022 build configuration as it's the latest version supported by Premake. However, you can use any version of Visual Studio 2022 or newer—we're currently developing with Visual Studio 2026.

## Solution Setup

All solution and dependency generation is done through premake. Click on `GenerateProjects.bat` to generate `.sln` file.

## Third-Party Library Setup

Libraries are managed through git submodule. Run `git submodule update --init --recursive` in the root folder.

## ReSharper C++ Setup

ReSharper C++ is recommended for enhanced code analysis and refactoring support. Students can obtain a free educational license using their college email through the [JetBrains Student Program](https://www.jetbrains.com/community/education/#students).

After obtaining your license, download and install ReSharper C++ from the [JetBrains website](https://www.jetbrains.com/resharper-cpp/). During installation, select your installed version of Visual Studio to integrate ReSharper into your development environment.

## Doxygen Setup

After cloning the repository, install Doxygen based on your operating system. For Windows users, download and run the installer from the [official Doxygen website](https://www.doxygen.nl/download.html). Linux and macOS users can follow the installation instructions provided on the same page.

Once installed, navigate to the `docs/` folder in the repository and run: `doxygen`

This will generate the documentation using the provided Doxyfile configuration.

## Templates Installation

RM Engine uses a standardized format for license headers in all source and header files. Visual Studio templates implementing these headers are located in the `templates` folder within the main directory. Run the batch file corresponding to your Visual Studio installation's item templates directory. The script currently supports Visual Studio 2022 and 2026 (VS 18).

## Formatting

This project enforces Google-style C++ formatting via a repo-root .clang-format file. All contributors must format code using clang-format, not IDE-specific formatters.

### Visual Studio (VS 2026)

- Go to Tools → Options → Text Editor → C/C++ → Formatting
- Enable ClangFormat and ensure the default VS formatter is disabled for C++
- Use Format Document (C++) to apply .clang-format

### Visual Studio Code

- Install C/C++ (ms-vscode.cpptools)
- Ensure VS Code uses the repo config by setting: `"C_Cpp.clang_format_style": "file"`
- (Optional) Enable `"editor.formatOnSave": true`

### ReSharper (Visual Studio)

- Go to ReSharper → Options → Code Editing → C++ → Formatting Style
- Set Formatter to ClangFormat
- Enable Use .clang-format file from project

> Formatting must always follow the committed .clang-format. IDE-specific styles must not override it.
