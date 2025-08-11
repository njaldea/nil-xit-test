# Tests

`XIT_TEST_F` uses a suite type that lists needed frame ids. Shorthand macros avoid the verbose `using` aliases:

```cpp
// Each template argument is a STRING LITERAL frame id (same literal as in XIT_FRAME_* macro)
XIT_INPUTS("config", "other_input");    // optional
XIT_OUTPUTS("circle");                  // optional
XIT_EXPECTS("expected_circle");         // optional
void setup();    // optional per‑test setup (runs before body)
void teardown(); // optional per‑test teardown (runs after body)
```

Skip any list you do not need (they default empty). `setup` / `teardown` are optional hooks.

```cpp
struct Suite {
  XIT_INPUTS();   // empty (no registered input frames referenced)
  XIT_OUTPUTS();  // empty
  XIT_EXPECTS();  // empty
  void setup();    // optional
  void teardown(); // optional
};

XIT_TEST_F(Suite, Case, "$group/folder") // third arg: group / UI path (if used)
{
  // Access via xit_inputs / xit_outputs / xit_expects
  auto& [config] = xit_inputs;              // first (and only) input frame
  auto& [circle] = xit_outputs;             // first output frame
  auto& [expected_circle] = xit_expects;    // first expect frame
  compute_circle(config, circle);
  ASSERT_EQ(expected_circle.radius, circle.radius);
}
```

Use structured bindings or `get<N>`; order matches the alias list. See [Binding](./04-BINDING.md).

## Provided Tuples
You can see up to three tuple objects:
- `xit_inputs`  : test + global inputs
- `xit_outputs` : outputs (you write)
- `xit_expects` : expected values (you may adjust when finalizing)

Example:

```cpp
auto& [cfg, other] = xit_inputs;          // structured binding (order = order of IDs in Inputs<...>)
auto& first_output = get<0>(xit_outputs);   // ADL finds nil::xit::gtest::get (no namespace needed)
```

If a category is unused it is an empty wrapper.
Main / utility frames are separate.
## Test Ids / Paths / Parameterization / Groups
Format: `SUITE.CASE[$group:relative_path]` (from 3rd macro arg)

Examples:
```
XIT_TEST_F(MySuite, Basic, "$demo/foo")       // MySuite.Basic[$demo:foo]
XIT_TEST_F(MySuite, Scan,  "$demo/data/*")    // one per subdir in $demo/data
```
Parameter folders: add `/*` → one test per immediate subfolder.
Groups: `$demo` comes from command line: `--group "demo=/abs/path"`.
Loaders get the tag so they can read/write per test data.

## Assertions
Use normal GoogleTest assertions.

## Minimal Example
```cpp
// Register frames (IDs are string literals)
XIT_FRAME_TEST_INPUT("config", make_config_loader());
XIT_FRAME_EXPECT("expected_circle", make_expected_circle_loader());
XIT_FRAME_OUTPUT("circle", Circle); // Outputs take the produced value type as template arg(s)

struct CircleSuite {
  XIT_INPUTS("config");
  XIT_OUTPUTS("circle");
  XIT_EXPECTS("expected_circle");
};

XIT_TEST_F(CircleSuite, RadiusMatches, "$group/test")
{
  const auto& [config] = xit_inputs;
  auto& [circle] = xit_outputs;
  auto& [expected_circle] = xit_expects;
  compute_circle(config, circle);
  EXPECT_EQ(expected_circle.radius, circle.radius);
}
```

Leave out categories you do not need.

## Plain `XIT_TEST`
`XIT_TEST` gives empty tuples; use `_F` if you need frames.
