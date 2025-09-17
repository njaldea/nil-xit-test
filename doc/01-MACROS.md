# Macros

Small helper macros to register data slots (frames).
`_V` form adds a UI component path.
Loaders (optional) can set and react to changes (may save, log, transform, etc.).

Rules:
- First argument: unique string literal id
- Input / Expect / Utility: last arg = seed value or loader
- Output: list of types (no loader)

## Frame registration

Main (optional root / layout):
- `XIT_FRAME_MAIN("$group/Main.svelte");`

Test input (per test):
- `XIT_FRAME_TEST_INPUT("config", Config{...});`
- `XIT_FRAME_TEST_INPUT_V("config", "$demo/Config.svelte", Config{...});`

Global input (shared across tests):
- `XIT_FRAME_GLOBAL_INPUT("shared_cfg", Config{...});`
- `XIT_FRAME_GLOBAL_INPUT_V("shared_cfg", "$demo/SharedCfg.svelte", Config{...});`

Expect (per test expected value):
- `XIT_FRAME_EXPECT("expected_circle", ExpectedCircle{...});`
- `XIT_FRAME_EXPECT_V("expected_circle", "$demo/ExpectCircle.svelte", ExpectedCircle{...});`

Output (produced by test code):
- `XIT_FRAME_OUTPUT("result", ResultType);`
- `XIT_FRAME_OUTPUT_V("result", "$demo/Result.svelte", ResultType);`

Utility (UI only, holds no data logic):
- `XIT_FRAME_TEST_V("marker", "$demo/Marker.svelte");`
- `XIT_FRAME_GLOBAL_V("gmarker", "$demo/GMarker.svelte");`

## Test declaration

Suite lists the frame ids it wants (shorthand macros):
```cpp
struct MySuite {
  XIT_INPUTS("config","shared_cfg");    // optional
  XIT_OUTPUTS("result");                // optional
  XIT_EXPECTS("expected_circle");       // optional
  void setup();    // optional
  void teardown(); // optional
};

`XIT_TEST` (no `_F`) = plain test (no frames).

## Suite frame list macros (`XIT_INPUTS` / `XIT_OUTPUTS` / `XIT_EXPECTS`)

Inside a suite struct you declare which registered frame IDs you want via three optional macros:

| Macro              | Expands To                                             |
|--------------------|--------------------------------------------------------|
| `XIT_INPUTS(...)`  | `using input_frames  = nil::xit::gtest::Inputs<...>;`  |
| `XIT_OUTPUTS(...)` | `using output_frames = nil::xit::gtest::Outputs<...>;` |
| `XIT_EXPECTS(...)` | `using expect_frames = nil::xit::gtest::Expects<...>;` |

Notes:
- All arguments are string literal frame IDs exactly as used in the `XIT_FRAME_*` registration macros.
- Omit a macro (or call it with no arguments: `XIT_INPUTS()`) to have an empty list for that category.
- Order matters: structured bindings / `get<N>` follow the order you list.
- Macros only introduce a single `using` alias; they do not allocate or access data themselves.
- You can freely mix categories; none are required.

Minimal pattern:
```cpp
struct S
{
    XIT_INPUTS("cfg");
    XIT_OUTPUTS("out");
    XIT_EXPECTS("expected_circle");
};

XIT_TEST_F(S, Run, "$g/demo")
{
    const auto& [cfg] = xit_inputs;         // from XIT_INPUTS
    auto& [out] = xit_outputs;              // from XIT_OUTPUTS
    auto& [expected_circle] = xit_expects;  // from XIT_EXPECTS
}
```

## Reserved frames

Some frame IDs are reserved by the test app/runtime and are created for you. Avoid registering frames with these IDs.

- index (unique): Root UI/index frame. App populates:
  - tags: comma-separated list of installed tags
  - finalize(tag): signal to trigger finalize over inputs of a tag
- frame_info (tagged): Per‑tag info frame exposing:
  - inputs: comma-separated IDs registered for the tag
  - outputs: comma-separated IDs registered for the tag
  - expects: comma-separated IDs registered for the tag
- tag_info (tagged): Output<bool> indicating a tag’s test status

## UI path guidance

UI component paths: just identifiers. Keep short.

## Summary table

| Category       | Macro                          | Per test? | Loader? | Flow    |
|:---------------|:-------------------------------|:----------|:--------|:--------|
| Main           | `XIT_FRAME_MAIN`               | n/a       | no      | input   |
| Test Input     | `XIT_FRAME_TEST_INPUT(_V)`     | yes       | yes     | input   |
| Global Input   | `XIT_FRAME_GLOBAL_INPUT(_V)`   | no        | yes     | input   |
| Expect         | `XIT_FRAME_EXPECT(_V)`         | yes       | yes     | expect  |
| Output         | `XIT_FRAME_OUTPUT(_V)`         | no        | no      | output  |
| Utility (test) | `XIT_FRAME_TEST_V`             | yes       | seed    | UI only |
| Utility (glob) | `XIT_FRAME_GLOBAL_V`           | no        | seed    | UI only |

Loader? = can take value / loader object (outputs ignore).
