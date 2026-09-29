# Angband 4.2.6 engine for AnybandUI

This branch implements AnybandUI protocol 1.0, profile `full-v1`, as a normal standalone process. It uses standard input/output JSON messages, without injection or instrumentation.

Build on Windows with Visual Studio C++ tools, Python and CMake:

```
python -B anybandui/build.py --ninja
python -B anybandui/test_backend.py --backend build-anybandui-native/game/angband-anybandui.exe
python -B anybandui/package_engine.py
```

The package appears in `build-anybandui-native/packages/angband-4.2.6`. Copy that folder into `engines` beside AnybandUI.exe and choose Rescan engines. AnybandUI and this engine have independent builds; neither requires the other repository to compile.

The authoritative compatibility contract lives in AnybandUI's `protocol/` directory. `full-v1.json` here is a vendored copy of its required capability list. Protocol messages and game-facing implementation are in `anybandui/backend/`. The adapter is selected by the `SUPPORT_ANYBANDUI_FRONTEND` build option. General engine queries and events are available to other frontends; optional interaction callbacks retain stock behaviour when unset. No `USE_ANYBANDUI` guard or binary ABI is required. See [the surface audit](../docs/anybandui-engine-integration.md) for measurements, responsibilities and upstream review boundaries.

Game data belongs to this package. User data is supplied by the frontend with `--user-dir` and must never be included in a distribution. The frontend isolates saves by engine identity and save compatibility identifier.

`build-ui-migration` is an ignored local archive of the former monolithic checkout and test data. It is not used by either build.
