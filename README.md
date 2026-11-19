# ForgeFlow v0.5 — belt orientation and visible production

**What changed:** Both placed and ghost conveyor models now receive the same +90° correction to the Kenney mesh, independently of the compass-direction arrows. Placed structures and ghost previews share one model-yaw function. This corrects the screenshot's visibly sideways conveyor geometry without changing simulation routing. The demo starts the simulation automatically; moving belt cargo appears as amber ore and cyan iron bars.

**Testing:** Select Demo (auto-start), watch the first belts for amber ore after approximately two seconds and the downstream belts for cyan bars after smelting; the stored-bars counter should increase. F3 toggles connector debug. Check all four belt orientations by pressing R.

**Limitation:** The Qt 3D view was not executed in this environment; the C++ core is tested independently. This update fixes the orientation discrepancy seen in the screenshot, but final visual verification in Qt Creator remains necessary.

---

# ForgeFlow — Factory Sandbox (v0.5.0)

A **C++17 + Qt Quick 3D** factory builder for the C++ final assignment. A first playable **prototype**, not a complete Satisfactory-like game.

> **Project status:** the platform-independent C++ core compiles and passes automated tests. The Qt Quick 3D application was **started successfully on Windows / Qt 6.11.2** by the user for v0.1. The user confirmed that v0.2 starts and that the Kenney palette renders. This v0.5 update addresses user-reported conveyor alignment, ghost connector visibility, reusable build tools and the Miner model. **The v0.5 GUI has not yet been tested in Qt Creator.** The C++ core and its automated tests have passed.

## What already works (intended GUI behaviour)

- A freely explorable **24×24 grid** with 4 fixed mineral deposits, no fog-of-war or map-unlocking.
- 3D angled camera with **middle mouse panning**, right-button orbiting and wheel zooming.
- Kenney **Factory Kit** GLB assets for the world, machines and conveyor belts, with their original Kenney colormap texture **embedded inside every GLB**. v0.1 inadvertently omitted this external texture, causing white models.
- Kenney **UI Pack — Sci-fi** visuals for the sidebar and buttons.
- Select **Miner (Kenney crane without optional magnet), Smelter, Storage or Conveyor**; place with left mouse click, rotate by **90° with R**; the chosen tool stays selected after successful placement, allowing repeated left-click placement. Right-click on the world or press Esc to cancel a tool. Clicking an existing structure with no tool active opens its live inspection panel.
- Ghost previews always show green input and orange output connectors; belts additionally show a cyan flow arrow. Placed structures hide these guides by default; press **F3** or use **Direction debug** to show them. Hold **Shift** while using the Conveyor tool: cursor movement previews a whole **straight line**. Click once to commit all cells. This option is *not* available for Smelters or any other machine. No click-and-drag required.
- Preview marks valid/invalid cells, rejects an entire blocked line, detects occupied cells; miners must be placed on resource deposits.
- Basic production simulation: Miner (1 ore/2s) → belts (0.65s/tile) → Smelter (1 bar/3s) → belts → Storage. Smelter and Storage accept input only from the back of their logical facing direction (green input marker); Miner/Smelter output forward (orange output marker).
- Start / Pause, demonstration factory, clear, save and load (JSON under application data directory), item and production counters.
- Unit tests for placement, rotation, clone/copy and ore-to-bar production, independent of Qt.

## v0.5 changes (against v0.3)

- Tool selection persists after placing one or more structures. Right-click or Esc exits build mode; clicking the selected sidebar button again also deselects it.
- All ghost previews show ports: **green input**, **orange output**, and, for belts, **cyan travel direction**. These are shown independently of the placed-structure debug option.
- **F3** toggles direction markers on already-placed buildings; they are hidden by default.
- Corrected the shared simulated world-direction transform for placed belts, the preview, and moving conveyor items. 90° rotations must be checked visually on the player's Qt/Windows build.
- Miner visual switched from `scanner-low.glb` to Kenney `crane.glb` (the separate `crane-magnet.glb` attachment is not used). The crane is scaled down for the 1×1 prototype footprint; this is a temporary visual compromise.

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
| Select a build tool | Sidebar or 1 / 2 / 3 / 4 |
| Place a single construction and exit build mode | Left click on a vacant location |
| Inspect an existing construction and live inventory | Left click with no tool selected |
| Cancel current tool / close inspection | Right click on field or Esc |
| Select Miner / Smelter / Storage / Conveyor | Sidebar or 1 / 2 / 3 / 4 |
| Rotate selected structure 90° | R |
| Conveyor-only line preview | Hold Shift and move mouse |
| Place whole preview line | Shift + left click |
| Cancel preview | Release Shift / Esc |
| Demolition tool | Click **DEMOLITION TOOL [X]** in left sidebar or press **X**; hover a building for red highlight; left-click to delete |
| Cancel demolition | Right click on field, press Esc, or press X again |
| Toggle direction/I-O indicators | **Direction + I/O markers** checkbox in left sidebar |
| Delete at cursor | Delete key while demolition mode is active (or left click) |
| Pan map | Middle mouse drag |
| Orbit camera | Right mouse drag |
| Zoom | Wheel |
| Pause/play | Space |
| Save / load | Ctrl+S / Ctrl+O |
| Help | H |

**Notes:** line placement is straight along the dominant X or Y axis; corners, splitters and automatic routing are not included yet. The **first cell** is captured when Shift begins at the current cursor position; left click commits the preview. A failed line placement leaves the map unchanged. All currently placed models use a **1×1 logical grid footprint**; visual scale is provisional. Camera panning is now computed relative to the camera yaw, with the drag direction designed to follow the view. Please test after rotating the camera.

## v0.3 quick acceptance test

1. Press **Demo** then **Play** and verify the storage counter increases. Green means an input connector, orange an output, cyan a conveyor travel-direction marker.
2. Click **DEMOLITION TOOL [X]** in the left sidebar. Hover a belt and verify a red cell highlight appears. Left-click to remove one segment. Right-click or X to exit demolition.
3. Select Conveyor, press R four times over a free tile. Ghost returns to its starting direction. Place a segment and verify its cyan direction corresponds to the ghost.
4. Select Conveyor, hold Shift over a start tile, move left/right/up/down, and inspect the preview. The resulting segments must point **along the dragged line**, not at 90 degrees. Place each line and check the cyan arrows.
5. Press Demo, left-click on Smelter and Storage in idle mode, inspect facing and contents. For an east-facing machine the orange output is on the east side; green input is on the west side.
6. Save, Clear, Load. Verify orientations, inventory and overlays are preserved.
7. Check the Kenney textures still render correctly and test the camera (middle-drag pan/right-drag orbit).

**Important:** user-facing 3D visual validation on Windows is still needed. The core C++ tests do pass, including orientation mapping and incorrect-side feeding. The current 3D models still have 1×1 logical footprints, and belts do not yet use automatic 90-degree corner models.

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
├── assets/factory/         Selected Kenney GLB assets with original texture embedded
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
- Conveyor direction is explicit, but curved mesh geometry, splitters, and automatic connector snapping/routing are not implemented. The first release of strict back-side inputs applies to Smelter and Storage; conveyor side-entry may still work for bends. The current 3D visual axis correction needs Windows confirmation.
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
