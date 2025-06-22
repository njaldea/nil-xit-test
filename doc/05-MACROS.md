# MACROS

MACROS for registering tests and frames.

> **Related documentation:**
> - [Concepts: Test Structure](./03-concepts.md#test-structure) - Test organization
> - [CLI Options](./04-cli.md) - Running tests
> - [Frame Bindings](./07-frame-bindings.md) - Binding data to UI components

## XIT_TEST

```cpp
XIT_TEST(Suite, Case, "$group/.")
{
    // test body
}
```

Registers a test without input/output.

The 3rd argument specifies the group path for test generation. This creates a test named `Suite.Case[group:.]`.

**Wildcard paths:** If the path ends with `*`, multiple tests are created for each subdirectory. For example, if group contains folders `a` and `b`, these tests are registered:
- `Suite.Case[group:./a]`
- `Suite.Case[group:./b]`

This enables easier parameterized testing by loading different inputs based on the path.

## XIT_TEST_F

```cpp
using Suite = nil::xit::gtest::Test<
    nil::xit::gtest::Input<...>,
    nil::xit::gtest::Output<...>
>;

XIT_TEST_F(Suite, Case, "$group/.")
{
    // test body
}
```

Registers a test with input/output frames.

## XIT_FRAME_MAIN

```cpp
XIT_FRAME_MAIN("$group/File.svelte")

// or with converter

XIT_FRAME_MAIN("$group/File.svelte", Converter)
```

`Converter` can be a functor or type that converts `std::vector<std::string>` to a serializable type. Used for advanced cases where the UI needs to query available tests and their inputs/outputs.

## Input Frame Registration

### XIT_FRAME_TEST_INPUT / XIT_FRAME_GLOBAL_INPUT

```cpp
XIT_FRAME_TEST_INPUT("test_frame_id", initializer());
XIT_FRAME_GLOBAL_INPUT("global_frame_id", initializer());
```

Registers frames without UI files for test input.

### XIT_FRAME_TEST_INPUT_V / XIT_FRAME_GLOBAL_INPUT_V

```cpp
XIT_FRAME_TEST_INPUT_V("test_frame_id", "$group/File.svelte", initializer());
XIT_FRAME_GLOBAL_INPUT_V("global_frame_id", "$group/File.svelte", initializer());
```

Registers frames with UI files for test input.

### XIT_FRAME_TEST_V / XIT_FRAME_GLOBAL_V

```cpp
XIT_FRAME_TEST_V("test_frame_id", "$group/File.svelte");
XIT_FRAME_GLOBAL_V("global_frame_id", "$group/File.svelte");
```

Registers frames with data and UI files.

## Output Frame Registration

### XIT_FRAME_OUTPUT (without UI)

```cpp
XIT_FRAME_OUTPUT("output_frame_id", DataType)
```

Registers output frames without UI files.

### XIT_FRAME_OUTPUT_V (with UI)

```cpp
XIT_FRAME_OUTPUT_V("output_frame_id", "$group/File.svelte", DataType)
```

Registers output frames with UI files.

> **See also:** [Custom Types](./06-custom-types.md) and [Frame Bindings](./07-frame-bindings.md) for detailed usage examples.