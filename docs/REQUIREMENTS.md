# Rubric coverage tracker — v0.2.0

**Important:** `implemented` only means there is a defensible code example. GUI items are **not verified** until the Windows Qt build is tested. Be precise in the final version; don't claim marks for a feature that doesn't work.

| # | Requirement | Status / evidence |
|---|---|---|
| 1 | Whole project UML | Partial: `docs/ARCHITECTURE.md` is a current-scope diagram, not full planned project |
| 2 | Abstraction | Implemented: `Building` API |
| 3 | Encapsulation | Implemented: nonpublic Building state |
| 4 | Inheritance | Implemented: four Building derivatives |
| 5 | Polymorphism | Implemented: mixed `tick` virtual calls in `Factory` |
| 6 | Object composition | Implemented: factory owns building instances; Qt controller owns factory |
| 7 | Base class | Implemented: `Building` |
| 8 | Abstract base | Implemented: pure virtual functions |
| 9 | Virtual function | Implemented: `tick`/`kind`/`clone` |
| 10 | No OOP errors | Requires complete audit before submission |
| 11 | Clean main | Implemented: `src/main.cpp` bootstrap |
| 12 | No globals | Implemented: no mutable global state |
| 13 | Header protections | Implemented: `#pragma once` in headers |
| 14 | Maintainability | In progress; extract game rules, code style pass |
| 15 | Separate headers | Implemented: `.h` and `.cpp` files |
| 16 | Compiles/no crash | Core tested; Qt GUI not tested here |
| 17 | Fully working | Partial prototype; GUI smoke test pending |
| 18 | +/- weekly Git commits | Pending: user starts Git history |
| 19 | Correct files in Git | Pending: user first commit; `.gitignore` supplied |
| 20 | README clean-PC build | Instructions written; clean Windows PC test pending |
| 21 | 2 default constructors | Implemented: Miner and Smelter |
| 22 | 2 parameterized constructors | Implemented: Building and Miner (plus Smelter) |
| 23 | 2 copy constructors | Implemented: Miner and Smelter; cloning demonstrated |
| 24 | 2 destructors | Implemented: Building and Miner; useful virtual destructor |
| 25 | Member init list | Implemented: Building/Miner/Smelter |
| 26 | Constructor forwarding | Implemented: delegating constructors in Miner/Smelter |
| 27 | Dynamic polymorphism | Implemented and unit-tested via production |
| 28 | Essential use of `this` | Not implemented; avoid decorative use |
| 29 | Useful member function | Implemented: `Factory::place` |
| 30 | Default function argument | Implemented in `Storage` and `Conveyor` ctor; clarify definition/declaration interpretation with teacher |
| 31 | Useful member variable | Implemented in all classes |
| 32 | Useful getters/setters | Implemented: `Building::cell`/`setCell`, `setFacing` |
| 33 | Inline function | Partial: methods defined in headers, needs explicit explanation/comment |
| 34 | Template function/class | Not yet implemented |
| 35 | Friend function/class | Not yet implemented |
| 36 | Useful namespace | Implemented: `forgeflow` |
| 37 | 2 memory-efficient types | Implemented: `Kind` and `Direction` backed by `uint8_t` (justify) |
| 38 | 4 const refs variables | Implemented: multiple `const auto&` loop/reference variables in `Factory.cpp`; documentation may need more specific examples |
| 39 | 4 const refs functions | Implemented: operator==, operator!=, setError and Qt model functions; defend 4 distinct parameter references |
| 40 | 2 smart pointers | Partial: unique_ptr + make_unique in many places; rubric interpretation needs second distinct useful demonstration |
| 41 | `new` | Not implemented (smart pointers preferred) |
| 42 | `delete` or kill children | Not yet claimed (destruction via unique_ptr; clarify with teacher) |
| 43 | 2 modern call-by-references | Partial, more explicit examples needed |
| 44 | String class | Implemented: std::string name/error |
| 45 | Container | Implemented: vector of buildings/deposits |
| 46 | nullptr | Implemented: missing Building in `Factory::at` |
| 47 | Modern file I/O | Implemented in Qt: QSaveFile + QJsonDocument, GUI untested |
| 48 | Exceptions | Implemented: safe load with exceptions, GUI untested |
| 49 | Lambda | Implemented: QObject timeout signal, std::find_if/any_of |
| 50 | Threads | Not implemented; QTimer is **not** a thread |
| 51 | Self-made Qt class | Implemented in source: GameController/BuildingModel, GUI untested |
| 52 | Signals/slots | Implemented in source: timer signals/slot and notify, GUI untested |
| 53 | TDD/test plan/tests | Implemented: actual C++ core unit tests, passed |
| 54 | Solve issue branch/link | Not done; real issue needed after initial commit |
| 55 | Report bug on other project | Not done; actual external report required |
| 56 | Meaningful GUI | Prototype implemented in QML; needs interactive validation |
| 57 | OpenGL or 3D engine | Qt Quick 3D in QML; verify acceptance with lecturer; GUI untested |
| 58 | External non-Qt library | Not done; Kenney images do NOT count |
| 59 | Hardware communications | Not done |
| 60 | Nice extra | Potential: grid-based conveyor-line preview; teacher assessment pending |

## Comments placement and exact markers

Use the teacher's requested comment heading **`Vereiste X: ...`** followed by a **specific explanation directly at the relevant implementation**. Headers, tests and controllers already contain starting annotations. Add missing explanatory markers when their requirement is genuinely met, and avoid fake placeholders in code.

The assignment document also indicates **red** criteria need no markers in code (10, 11, 12, 14–19) and **green** criteria belong in README (20, 54, 55). This tracker is documentation, not a substitute for the actual mandatory inline comments.
