# nil-xit gtest Addon

Add simple editable data blocks to GoogleTest.

## Why
- Make inputs / outputs / expects tracked
- Replace random literals & loose golden files
- Let you tweak data visually and review diffs

## Core Idea
- Register frames once (macros)
- A suite (struct) lists frame ids it needs
- In test: use `xit_inputs`, `xit_outputs`, `xit_expects`
- Optional loader functions: `initialize`, `update`, `finalize` (your code decides what they do)
- Optional GUI to edit; headless just runs

## When It Shines
- Golden / baseline updates
- Parameter tuning loops
- Visual / structured output (JSON, shapes, metrics)
- Data that changes over time

## Key Terms
- Frame: named value (string id)
- Loader: code that sets start value; optional functions can store or change it
- Binding: members shown in UI
- Tag: unique test id part

## Quick Example
```cpp
#include <nil/xit/gtest.hpp>

XIT_FRAME_TEST_INPUT("ranges", Ranges{1,2,3});
XIT_FRAME_EXPECT("expected", nlohmann::json{{"y", {1,2,3}}});
XIT_FRAME_OUTPUT("scaled", nlohmann::json);

struct Fix {
  XIT_INPUTS("ranges");
  XIT_EXPECTS("expected");
  XIT_OUTPUTS("scaled");
};

XIT_TEST_F(Fix, run, "$demo/example/*") {
  const auto& [r] = xit_inputs;
  auto& [expected] = xit_expects;
  auto& [out] = xit_outputs;
  out = {{"y", {r.v1, r.v2, r.v3}}};
  EXPECT_EQ(expected, out);
}
```

Run headless (default) or with GUI:
```
./tests gui --group "$demo=/abs/path"
```

Test id details: [03-TESTS](./doc/03-TESTS.md#test-ids--paths--parameterization--groups)
Mnemonic:
- Format: `SUITE.CASE[$group:path]`
- Add `/*` for each subfolder

## Minimal Workflow
1. Register frames
2. Add frame ID aliases to suite
3. (Optional) Edit in GUI
4. Run tests (update / finalize loaders as needed)
5. Commit data + code

## Docs
1. [Macros](./doc/01-MACROS.md)
2. [Frames](./doc/02-FRAMES.md)
3. [Tests](./doc/03-TESTS.md)
4. [Binding](./doc/04-BINDING.md)
5. [Loaders](./doc/05-LOADERS.md)
6. [Pros & Cons](./doc/06-PROS_CONS.md)

Examples: `doc/test/`
