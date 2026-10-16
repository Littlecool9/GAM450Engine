# Mitosis Engine · Aural Mischief

**C++20 / OpenGL custom game engine and rhythm action platformer — Jackey Cen's team-project portfolio.**

![Aural Mischief gameplay](submission/AuralMischief_1.jpg)

Aural Mischief is a rhythm action platformer built by Team Olive at DigiPen, powered by the team's Mitosis engine. Players navigate platforms and enemies with a guitar-wielding hero, timing actions to the soundtrack.

My work spans **engine systems, rendering, UI programming, and level design**. The submission credits me as Game Designer; my commits also document engine and gameplay programming. This is a collaborative project.

Read the illustrated **[中文个人贡献介绍](PORTFOLIO.zh-CN.md)** and the **[original contribution record](docs/JACKEY_CONTRIBUTIONS.md)**.

## My contributions

| Area | Contribution | Code |
| --- | --- | --- |
| Objects/components | GameObject identity, tags/layers, factories, ownership changes, UUID refactoring | [GameObject](Mitosis-Engine/src/Engine/EntitySystem/GameObject/GameObject.h) |
| Scenes/events | Scene switching, persistent objects across scenes, templated delegates | [SceneSystem](Mitosis-Engine/src/Engine/SceneSystem/SceneSystem.cpp) |
| Rendering | Command-submission renderer refactor and camera-based background parallax | [Renderer](Mitosis-Engine/src/Engine/Renderer.cpp) |
| Post-processing | Framebuffers, ping-pong effect stack, tint, brightness/contrast/saturation, Gaussian blur, frame blending for motion blur, inspector controls | [PostProcessStack](Mitosis-Engine/src/Engine/Renderer/PostProcessStack.cpp) |
| Menus/HUD | Main/pause menus, relative screen-space UI, keyboard/gamepad/mouse navigation, modal focus, audio controls, health HUD | [MenuNavigator](AuralMischief/src/Script/MenuNavigator.cpp), [PauseMenuNavigator](AuralMischief/src/Script/PauseMenuNavigator.cpp) |
| Playable content | Prototype iteration, level layouts, second-level work, checkpoints and polish | [Scenes](AuralMischief/Data/Scenes) |

![Engine editor and playable scene](submission/AuralMischief_tech.jpg)

*Original team screenshot; editor panels and visible assets are team work. This is not a claim of sole authorship or a new runtime verification.*

## Technology and layout

C++20, OpenGL/GLAD, GLFW, GLM, Dear ImGui, RapidJSON, Boost.UUID, spdlog, FMOD, vcpkg, and GoogleTest (test project).

- `Mitosis-Engine/`: engine and editor.
- `Mitosis-Launcher/`: application launcher.
- `AuralMischief/`: game scripts, scenes, prefabs, runtime assets.
- `Mitosis-Tests/`: existing tests; absent from the current top-level solution.
- `archive/`: earlier prototypes.
- `submission/`: original screenshots, assets, third-party license materials.

## Build setup

The checked-in projects target **Windows x64, C++20, MSVC toolset v145**. Use Visual Studio with that toolset, or explicitly retarget the projects. Retargeting has not been validated for this export.

```powershell
git clone --recurse-submodules https://github.com/Littlecool9/GAM450Engine.git
cd Mitosis-Engine-AuralMischief
.\vcpkg\bootstrap-vcpkg.bat
```

Install the FMOD Studio API for Windows. Project files reference Core/Studio headers, x64 libraries, and DLLs under `C:\Program Files (x86)\FMOD SoundSystem\FMOD Studio API Windows`; adjust paths if needed. Some configurations also contain machine-specific paths.

Open `Mitosis-Engine.sln`, select x64 and the desired configuration, and build `AuralMischief`. `Directory.Build.props` and `Directory.Build.targets` configure vcpkg manifest integration. Check debugger working-directory and asset-copy settings in `AuralMischief.vcxproj` when running from the IDE.

The inherited README mentioned Premake scripts absent from the final tree; use the checked-in solution. **This export has not been compiled or playtested.** Screenshots are from the original submission.

## Team, provenance, and rights

Original: [DigiPen-GAM5xx/gam541-olive](https://github.com/DigiPen-GAM5xx/gam541-olive) (access may be restricted). Snapshot: `0a4d9db17e6fc1c8448cd9395a8c28e744e4babf`.

| Contributor | Original submission credit |
| --- | --- |
| Rahul Nair | Producer |
| Sumin Park | Technical Lead |
| Josh Siegel | Graphics Programmer |
| Jackey Cen | Game Designer |
| Quy Dong Ta | AI Behavior |
| Griffin Hart | Composer |

This personal repository preserves the complete original main-branch history and original project files, including submission documents, the team contact sheet, and GitHub automation. Original commit hashes, authors, dates, and messages are unchanged. Portfolio documentation is added as a subsequent commit. A local backup of the earlier public snapshot is retained.

Original DigiPen copyright notices and team/third-party credits are retained. No new open-source license is granted by this publication. See [third-party materials](submission/AuralMischief_licenses).
