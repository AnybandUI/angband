# Angband frontend integration: measured surface

The independent AnybandUI application owns the versioned full-v1 wire protocol.
This repository owns its Angband adapter and the small engine changes that make
that adapter possible. The C interface is source-level and Angband-specific, not
a cross-fork binary ABI. No pointers or engine enum layouts cross the process boundary.

## Reduction experiment, September 2026

Measured against pristine Angband 4.2.6 (`f3082213b`), including CMake:

| Existing-file patch | Before (`932249f74`) | After |
| --- | ---: | ---: |
| Files changed | 58 | 55 |
| Added lines | 625 | 589 |
| Removed lines | 97 | 97 |

These counts exclude separately added adapter files and documentation. Adapter
code grew slightly: policy belongs there instead of in existing engine files.
Run `python -B anybandui/audit_surface.py` to reproduce the current measurement;
add `--revision 932249f74` for the starting point. This measures changes relative
to stock, not merely the last commit. Line counts are evidence, not an API quality score.

### Changes actually removed

* `init.c` is restored to stock. The adapter reuses `constants_parser` and its
  init/run/finish/cleanup lifecycle instead of adding a second parser path and a
  frontend-specific global input buffer to engine initialization. `init.h` only
  declares the existing parser object, like the other exported file parsers.
* `message.h` is restored to stock. Sound cues use `EVENT_SOUND_CUE`; the existing
  `EVENT_SOUND` still respects the stock audio preference. Independent frontend
  playback therefore remains possible without changing ordinary audio behaviour.
* `target.h` is restored to stock. Successful explicit selection emits
  `EVENT_TARGET_SELECTED` through the existing event dispatcher. Tracking updates
  and cancelled selection do not emit it.

The parser reuse is deliberately an adapter technique, not a promised generic API.
The stock finish callback temporarily publishes `z_info`. Validation runs only at
synchronous adapter input boundaries: save the live pointer, parse and finish the
candidate, clean it up, then restore the live pointer on success AND failure.
There are no UI callbacks or gameplay commands during this operation. Changing the
parser lifecycle in a future engine version requires reviewing this adapter code.
A pure validation API would be cleaner for concurrency, but would increase the
engine surface; this implementation does not claim thread-safe validation.

## Remaining engine responsibilities

| Responsibility | Why the hook/query remains |
| --- | --- |
| Read-only known-map extraction | Rendering must not reveal unknown cells, memorize terrain or consume gameplay RNG. |
| Input context and borrowed item/effect information | Native prompts need the current legal action, not guesses from English terminal text. |
| Birth, inventory, equipment, book and store entry points | Replace presentation while retaining the engine's prerequisites and command validation. |
| Store transactions and targeting | Reuse the actual engine action paths; do not duplicate pricing or targeting rules. |
| Combat, motion and projection events | Report actual outcomes and distinguish walking, teleportation, arcs and explosions. |
| Death and continuation boundaries | Preserve final messages and transition at the correct engine boundary. |
| Structured descriptions and level feelings | Reuse the engine's calculations and wording instead of implementing another rules engine. |
| Keymap provenance and tile preferences | Preserve user bindings and constrain artwork imports to presentation. |

These are general frontend needs, though the present APIs still contain Angband
structures and assumptions. Combining every hook in one new header would reduce
file counts without eliminating those obligations. Parsing screen text or reading
unobserved game state would reduce patch size at the expense of correctness.

## Upstream review boundaries

Review independently, in this order:

1. Correctness fixes: wide arithmetic in object power and freeing the temporary
   object after cancelling a store purchase. These are not protocol requirements.
2. Read-only queries and structured presentation: map safety, descriptions and
   character-sheet data; useful to other frontends as well.
3. Engine observations: combat, actor motion, sound cues, target selection,
   projection metadata and lifecycle notifications using the existing event bus.
4. Frontend interaction hooks: native-screen substitution, prompt context and
   validated action entry points. Defaults retain the stock terminal path.
5. Optional AnybandUI adapter and its build switch. JSON, manifests, saved tuning,
   journal policy and frontend transport remain here, outside ordinary game logic.

This is a review plan, not a claim that each item has already been split into an
independently buildable patch series. No mainline acceptance is assumed.

## Verification and portability

`python -B anybandui/test_backend.py --backend BUILD/game/angband-anybandui.exe`
checks complete native workflows. The suite includes tuning rejection while a
character is running, per-save structural settings, sound independent of stock
playback, explicit target confirmation, cancellation and query purity.

AnybandUI's separate repository supplies protocol discovery and handshake checks,
frontend regressions and the formal requirements in `protocol/`. Full integration
remains required. A future fork translates its own objects, options, effects and
slots into that wire contract; it does not copy this adapter's C layouts blindly.

The current wire reference is maintained in AnybandUI, not the historical 0.1
proposal. Build and packaging instructions are in `anybandui/README.md`.
