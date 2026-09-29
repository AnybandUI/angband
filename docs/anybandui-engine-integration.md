# External frontend integration

The AnybandUI protocol adapter is maintained in the sibling
`AnybandUI-AngbandAdapter` project. This checkout contains Angband plus the
adapter's reviewed engine patch series, based on upstream 4.2-release commit
`f3082213b73f3e463e3d0d60bff4b00462beae6e`.

The embedded backend has been retired. JSON, manifests, presentation policy and
engine packaging belong to the external adapter. The UI still communicates
with one engine process using Anyband Protocol (`anyband-protocol`).

## Build

From a Visual Studio x64 developer shell in this checkout:

```powershell
$adapter = (Resolve-Path ../AnybandUI-AngbandAdapter).Path
cmake -S . -B ../AnybandUI-AngbandAdapter/build/external-native -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=RelWithDebInfo "-DANGBAND_EXTERNAL_FRONTEND=$adapter" -DSUPPORT_BORG=OFF -DSUPPORT_SPOIL_FRONTEND=OFF
cmake --build ../AnybandUI-AngbandAdapter/build/external-native --target OurExecutable anybandui-map-tests
```

Use a fresh build directory; an old embedded-backend cache is incompatible.
Ordinary platform frontends remain separate builds without the external option.

## Engine responsibilities

Angband owns rules, RNG, visibility, legal actions and saves. Its frontend
interfaces expose read-only known-map data, drawing layers, input callbacks,
existing calculations and observations of combat, movement and lifecycle events.
The adapter assembles protocol records and native interactions from those
interfaces. The integration is source-version-specific, not a stable binary ABI.

The authoritative patches and validation history live in the adapter's
`patches/` and `docs/` directories. The patch series also retains independent
correctness fixes and sound-event tests. See the adapter README for installation,
test coverage and outstanding automated-test limitations.
