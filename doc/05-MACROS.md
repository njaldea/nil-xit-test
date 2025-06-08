# MACROS

The library provides a variety of MACROS to register tests and frames.

> **Related documentation:**
> - [Concepts: Test Structure](./03-concepts.md#test-structure) - For understanding test organization
> - [CLI Options](./04-cli.md) - For running tests with proper arguments
> - [Frame Bindings](./07-frame-bindings.md) - For binding data to UI components

## XIT_TEST

```cpp
XIT_TEST(Suite, Case, "$group/.")
{
    // test body
}
```

Register a test without input/output.

The 3rd argument is a group and the path to be used to generate the test. See [Concepts: Group](./03-concepts.md#group) for details on group paths.

This will generate a test named `Suite.Case[group:.]`. See [Concepts: Test Identification](./03-concepts.md#test-identification) for more on test naming.

If the last path is an `*`, it will instead create multiple tests for the same tests.
Let's assume there are two folders inside group named `a` and `b`, the following tests should be registered:
- `Suite.Case[group:./a]`
- `Suite.Case[group:./b]`

This path is going to be used to load inputs that depend on a path, resulting in easier way of doing parameterized testing.

## XIT_TEST_F

```cpp

using Suite = nil::xit::gate::Test<
    nil::xit::gate::Input<...>,
    nil::xit::gate::Output<...>
>;

XIT_TEST_F(Suite, Case, "$group/.")
{
    // test body
}
```

Register a test with the set of inputs/outputs.

## XIT_FRAME_MAIN

```cpp
XIT_FRAME_MAIN("$group/File.svelte")

// or 

XIT_FRAME_MAIN("$group/File.svelte", Converter)
```

`Converter` could be a functor or a type that can convert `std::vector<std::string>` to a serializable type.
This is goign to be used for advanced use case where the UI needs to query about available tests and their
equivalent inputs/outputs

## XIT_FRAME_TEST_INPUT / XIT_FRAME_GLOBAL_INPUT

```cpp
XIT_FRAME_TEST_INPUT("test_frame_id", initializer());
XIT_FRAME_GLOBAL_INPUT("global_frame_id", initializer());
```

Registers a frame without an equivalent UI file to be used as an input to a test.

See [Custon Types](./06-custom-types.md) for more detail.
See [Frame Bindings](./07-frame-bindings.md) for more detail.

## XIT_FRAME_TEST_INPUT_V / XIT_FRAME_GLOBAL_INPUT_V

```cpp
XIT_FRAME_TEST_INPUT_V("test_frame_id", "$group/File.svelte", initializer());
XIT_FRAME_GLOBAL_INPUT_V("global_frame_id", "$group/File.svelte", initializer());
```

Registers a frame with an equivalent UI file to be used as an input to a test.

See [Custon Types](./06-custom-types.md) for more detail.
See [Frame Bindings](./07-frame-bindings.md) for more detail.

## XIT_FRAME_TEST_V / XIT_FRAME_GLOBAL_V

```cpp
XIT_FRAME_TEST_V("test_frame_id", "$group/File.svelte");
XIT_FRAME_GLOBAL_V("global_frame_id", "$group/File.svelte");
```

Registers a frame with an equivalent data.

See [Frame Bindings](./07-frame-bindings.md) for more detail.

## XIT_FRAME_OUTPUT

```cpp
XIT_FRAME_OUTPUT("output_frame_id", DataType)
```

Registers a frame without equivalent UI file to be used as an output of the test.

See [Custon Types](./06-custom-types.md) for more detail.
See [Frame Bindings](./07-frame-bindings.md) for more detail.

## XIT_FRAME_OUTPUT

```cpp
XIT_FRAME_OUTPUT("output_frame_id", "$group/File.svelte", DataType)
```

Registers a frame without equivalent UI file to be used as an output of the test.

See [Custon Types](./06-custom-types.md) for more detail.
See [Frame Bindings](./07-frame-bindings.md) for more detail.