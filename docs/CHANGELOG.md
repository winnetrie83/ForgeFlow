# Changelog

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
