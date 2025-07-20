# Binding Values & Members

Input / Output frames can show parts (members). See [Macros](./01-MACROS.md) and [Loaders](./05-LOADERS.md).

## Overloads (chainable):
Given a frame builder produced by one of:
- `XIT_FRAME_TEST_INPUT(_V)` / `XIT_FRAME_GLOBAL_INPUT(_V)`
- `XIT_FRAME_OUTPUT(_V)`

1. `value("id", &Type::member)` – bind a member (edit for input, view for output)
2. `value("id", accessor)` – accessor returns a reference
3. `value("id", unrelated_value)` – add extra stored value
4. `value("id")` – expose whole object 

## Example
```cpp
struct Config { int width{}; int height{}; };

XIT_FRAME_TEST_INPUT_V("config", "Step_4Main.svelte", Config{})
	.value("width",  &Config::width)
	.value("height", &Config::height);

// Expose entire object instead (single editable entry named "cfg")
XIT_FRAME_TEST_INPUT("cfg", Config{}).value("cfg");

// Global input adding an extra standalone flag not in Config
XIT_FRAME_GLOBAL_INPUT("gcfg", Config{})
	.value("width",  &Config::width)
	.value("height", &Config::height)
	.value("enabled", true); // independent persisted bool
```

## Meaning
- Input: edit before run; update/finalize do whatever loader defines (maybe save)
- Output: view after you assign
- Expect: internal expected data

## Default (Implicit) Binding
If you add no `.value(...)` and type has a codec, whole object auto-bound as `"value"`.

Explicit bindings suppress the implicit one. EXPECT frames skip implicit binding.

## Codec Requirement
Codec only needed for UI.

## Accessors
Example accessor:
```cpp
// Suppose Config has Nested nested; and Nested has int depth;
XIT_FRAME_TEST_INPUT("cfg", Config{})
	.value("depth", [](Config& c) -> int& { return c.nested.depth; });
```
Accessor must return a non-const reference.

## Guidance
- Pick members for fine control
- Whole object for simple blobs
- Large containers: bind container, not each element

## Duplicates
Avoid duplicate ids.

## Rationale
Bindings give an explicit list for UI and saving.

See how frames appear inside tests: [Tests](./03-TESTS.md). Frame kinds reference: [Frames](./02-FRAMES.md).
