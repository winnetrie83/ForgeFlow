## v0.5.1 — Responsive user interface
- Separate scrollable build/status sidebar from fixed world-control footer.
- Lower initial/minimum window dimensions; responsive HUD and inspector widths.
- No simulation or asset changes.

# Changelog

## v0.4.0 — Placement & visual directions

- Fixed build selection being deactivated after every placement. The active tool remains selected for repeat placements.
- Preview always renders logical input/output markers; belts additionally show flow arrows.
- Added a user-facing F3 directional debug toggle. Placed-structure overlays are off by default.
- Corrected the placed/preview transform convention (North=-Z, East=+X, South=+Z, West=-X) and conveyor item movement orientation.
- Changed Miner model to Kenney crane body without separate magnet attachment; resized and recentered it.
- Maintained v0.3 demolition tool and inspector.
- C++ core passes unit tests; v0.4 Qt GUI remains to be tested on Windows.


## v0.3.0 — demolition, direction markers and grid rotation

- Added a dedicated **DEMOLITION TOOL [X]** button to the build sidebar and a matching bottom-bar control, plus the X shortcut. Hovered occupied cells display a red removal highlight. Right-click / Esc cancels demolition.
- Added always-optional (checkbox) visual indicators: cyan travel chevrons on conveyors, green machine inputs, orange machine outputs. Ghost preview shows the conveyor direction.
- Corrected 90-degree mesh visual offset consistently for placed buildings and ghost models; the actual direction markers are based on the C++ grid directions, independent of Kenney mesh axes.
- Unified dominant-axis conveyor-line facing logic in the core (`directionForLine`) so previews and placement agree for north/south/east/west.
- Smelters and Storage now enforce their displayed rear input side in the simulation. The example production line still works.
- Added core regression tests for four line directions and rejection of a smelter connected on the wrong side.
- Qt Quick 3D UI requires user verification on Windows; engine compiled and CTest passed in the authoring environment.

## v0.2.0 — inspection and asset fixes

- Corrected relative paths for `qt_add_resources()` in CMake (fixes Qt 6.11 CMake configuration error).
- Fixed the `font.letterSpacing` QML usage that caused the initial startup failure.
- Fixed Kenney GLB color textures: the originals refer to an external `Textures/colormap.png` that was not copied into v0.1; the official colormap is now embedded in each bundled GLB.
- Build tool deactivates after successful placement; right-click or Esc cancels the selected tool.
- Left click with no build tool activates a live structure inspector (including current storage inventory).
- Replaced Storage visual from the small hopper with the `box-large` model.
- Removed right-click deletion and gated Delete behind Erase mode.
- Made camera panning camera-yaw-relative instead of fixed global-axis dragging.
- Added a test assertion for storage inventory after running the production simulation.
- Qt GUI needs Windows verification; C++ engine tests pass.


## v0.1.0 — initial prototype

- First C++17 factory simulation core, testable without Qt.
- Qt Quick 3D grid world and camera (panning/zoom/orbit).
- Kenney Factory Kit GLB models and predominantly Kenney Sci-fi UI visuals.
- Single placement, 90° rotation, conveyor-only Shift line placement preview.
- Miner, Smelter, Conveyor and Storage with item movement and counters.
- Demo factory, pause/play, atomic JSON save/load, clear factory.
- CMake/Qt Creator setup, UML, rubric tracker and automated core tests.
- Known: Qt GUI has not yet been run in a suitable test environment.

## v0.5.0 — Belt orientation and visible production
- Use one shared `meshYaw(facing, kind)` for real and preview 3D models; separate geometry from input/output/flow overlay yaw.
- Correct conveyor mesh longitudinal axis by +90° relative to its logical flow arrow.
- Rotate placed direction markers using their own logical facing; do not rotate their parent node.
- Increase contrast/size of cargo markers and start Demo simulation automatically.
- Add four-orientation source-to-smelter regressions and visible-item-to-storage regression.
