# ForgeFlow — Factory Sandbox (v0.1.0)

A **C++17 + Qt Quick 3D** factory builder for the C++ final assignment. A first playable **prototype**, not a complete Satisfactory-like game.

> **Project status:** the platform-independent C++ core compiles and passes automated tests. The Qt Quick 3D application has **not been run on the author's Windows/Qt installation yet**. Follow the build steps below, then test 3D scene loading, clicking and GLB asset rendering. Issues found in that first GUI test belong in GitHub Issues and the next iteration.

## What already works (intended GUI behaviour)

- A freely explorable **24×24 grid** with 4 fixed mineral deposits, no fog-of-war or map-unlocking.
- 3D angled camera with **middle mouse panning**, right-button orbiting and wheel zooming.
- Kenney **Factory Kit** GLB assets for the world, machines and conveyor belts.
- Kenney **UI Pack — Sci-fi** visuals for the sidebar and buttons.
- Select **Miner, Smelter, Storage or Conveyor**; place with left mouse click, rotate by **90° with R**; building mode stays active for the next click.
- Hold **Shift** while using the Conveyor tool: cursor movement previews a whole **straight line**. Click once to commit all cells. This option is *not* available for Smelters or any other machine. No click-and-drag required.
- Preview marks valid/invalid cells, rejects an entire blocked line, detects occupied cells; miners must be placed on resource deposits.
- Basic production simulation: Miner (1 ore/2s) → belts (0.65s/tile) → Smelter (1 bar/3s) → belts → Storage. Adjacent machine transfer also works.
- Start / Pause, demonstration factory, clear, save and load (JSON under application data directory), item and production counters.
- Unit tests for placement, rotation, clone/copy and ore-to-bar production, independent of Qt.

## Windows build from scratch (Qt Creator + MinGW)

1. Install **Qt 6.5 or newer**, preferably your existing **Qt 6.11 / 6.12** kit. During installation include **Qt Quick 3D**, **Qt Quick Controls**, **Qt QML**, **MinGW toolchain** (or compatible MSVC kit), **CMake**, **Ninja** and **Qt Creator**. The runtime GLB loader requires the QML module `QtQuick3D.AssetUtils` (supplied with the Qt Quick 3D installation).
2. Extract the ZIP to a simple path such as `C:\\Projects\\ForgeFlow`. Keep `assets/`, `qml/`, `src/`, `tests/` and `CMakeLists.txt` together.
3. Open **Qt Creator → File → Open File or Project → CMakeLists.txt**.
4. Select a **Desktop Qt 6.x** kit. Let Qt Creator configure CMake; check that it does **not** report missing `Quick3D` or `QtQuick3D.AssetUtils`.
5. Select build target **ForgeFlow**, build and run (Ctrl+R). Your graphics driver must support Qt Quick 3D rendering.
6. If the window appears but models don't, inspect **Application Output** for QML import or GLB loader errors. Confirm the `.glb` resources are present in the CMake configuration. If `QtQuick3D.AssetUtils` cannot be imported, install the Quick 3D component in the Qt Maintenance Tool.
7. Use **Demo**, then **Play**. Watch `In storage` increase. Use **Clear** to start from scratch.

### Alternative command-line build (Windows CMD)

Use the *same* Qt compiler kit for CMake and compiler (the commands below require CMake and Ninja on `PATH` and suitable `CMAKE_PREFIX_PATH`):

```cmd
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH="C:\Qt\6.11.2\mingw_64"
cmake --build build --config Debug
ctest --test-dir build --output-on-failure
```

**Change the Qt path to the version and kit installed on your machine.** For MSVC use an appropriate CMake generator/environment instead of MinGW/Ninja. A build on a clean Windows PC is not yet verified, so this README should be corrected after that test.

### Build and run the C++ core tests without Qt

```cmd
cmake -S . -B build-core -DFORGEFLOW_BUILD_GUI=OFF
cmake --build build-core
ctest --test-dir build-core --output-on-failure
```

This needs CMake and a C++17 compiler only. The core has been compiled and its tests run in the authoring environment.

## Controls

| Action | Input |
|---|---|
| Place a single construction | Left click |
| Select Miner / Smelter / Storage / Conveyor | Sidebar or 1 / 2 / 3 / 4 |
| Rotate selected structure 90° | R |
| Conveyor-only line preview | Hold Shift and move mouse |
| Place whole preview line | Shift + left click |
| Cancel preview | Release Shift / Esc |
| Delete at cursor | Right click / Delete key / Erase toggle |
| Pan map | Middle mouse drag |
| Orbit camera | Right mouse drag |
| Zoom | Wheel |
| Pause/play | Space |
| Save / load | Ctrl+S / Ctrl+O |
| Help | H |

**Notes:** line placement is straight along the dominant X or Y axis; corners, splitters and automatic routing are not included yet. The **first cell** is captured when Shift begins at the current cursor position; left click commits the preview. A failed line placement leaves the map unchanged. All currently placed models use a **1×1 logical grid footprint**; visual scale is provisional. The current camera pan uses a screen-relative, not fully camera-relative, translation and will be refined.

## Architecture

```text
ForgeFlow/
├── CMakeLists.txt
├── src/core/Factory.h       Pure C++ interfaces and factory model
├── src/core/Factory.cpp     Rules, machine simulation, transfers
├── src/qt/GameController.h  Qt-facing properties, model and actions
├── src/qt/GameController.cpp  QAbstractListModel, timer, JSON saves
├── src/main.cpp            Application bootstrap
├── qml/Main.qml            3D world, camera and sci-fi UI
├── assets/factory/         Selected original Kenney GLB assets
├── assets/ui/              Selected original Kenney sci-fi PNGs
├── tests/test_factory.cpp  Core automated tests
└── docs/                   UML, rubric and iteration plan
```

**Design principle:** the engine works entirely without Qt, while QML only renders and interacts with the engine through a C++ controller. Buildings use an abstract `Building` base class with virtual `tick()`/`clone()`, `std::unique_ptr` ownership and derived machine types. The shared world grid is translated to 3D positions in QML. See `docs/ARCHITECTURE.md`.

## Assignment requirements in the README (green items)

**Vereiste 20: working build manual as readme on GitHub (project must be possible to build from scratch on a clean PC)**

The Windows Qt Creator build manual above specifies dependencies, versions, toolchain, CMake setup, start and troubleshooting, including a non-Qt test build. **Pending:** verify the clean-PC claim by building on a fresh Windows installation; do not claim this criterion is completed before that check.

**Vereiste 54: solve bug ticket (with pull request or commit message issue link and issue branch)**

Not completed in v0.1. After the first Git commit, create an actual issue for a reproducible bug, branch from main (e.g. `fix/12-grid-placement`), resolve it, and link the issue in the commit message or PR. Do not invent tickets.

**Vereiste 55: report a bug ticket on another project**

Not completed in v0.1. Find and responsibly report a real, reproducible bug on a different project and document the link; comply with its contribution guidelines. Do not submit artificial reports.

## Known limitations / next releases

- **GUI untested here:** visual/interaction behaviour must first be tested in your installed Qt Creator/Qt Quick 3D environment.
- Very early gameplay: one resource (iron), one processing recipe, four building types; no research, budgets, power or progression mode yet.
- Belt segment direction is functional in the simulation, but corner geometry and dynamic automatic port/connector validation are not implemented.
- Single-cell footprints only; multi-cell machines, rotated footprints, decorative extras and selected-object inspector are future work.
- Default rendering may require tuning for laptop graphics; per-tick full model refresh is prototype-only.
- Qt Core tests run; QML smoke tests and clean Windows build still outstanding.
- A full game balance pass, save migrations, robust versioning and resource capacity are pending.

## Assets, ownership, and license

- **3D models:** [Kenney Factory Kit](https://kenney.nl/assets/factory-kit), original `.glb` assets selected from the user's provided archive.
- **UI:** [Kenney UI Pack — Sci-fi](https://kenney.nl/assets/ui-pack-sci-fi), original PNGs selected from the user's provided archive.
- Their original CC0 license texts are included in `assets/` (`KENNEY_FACTORY_LICENSE.txt`, `KENNEY_UI_LICENSE.txt`). Kenney artwork is third-party, C++/QML code is authored specifically for ForgeFlow. The existing asset collection was supplied by the user.

## First Git commit (suggestion)

```cmd
git init
git add .
git commit -m "feat: initialize ForgeFlow factory sandbox with C++ simulation and Qt 3D prototype"
```

The ZIP does **not** include a Git repository. Push to GitHub yourself. Keep the early TODOs honest: do not mark the GUI requirements as verified until the program has been run in Qt Creator.
