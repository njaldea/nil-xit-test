# Frame Types

| Frame Type   | Has Data? | Test writes? | UI edits? |
|:------------:|:---------:|:------------:|:---------:|
| Main         | No        | N/A          | N/A       |
| Test View    | No        | N/A          | No        |
| Global View  | No        | N/A          | No        |
| Test Input   | Yes       | No           | Yes       |
| Global Input | Yes       | No           | Yes       |
| Expect       | No        | Yes          | Finalize  |
| Output       | Yes       | Yes          | No        |

View frames are UI-only frame slots registered with `XIT_FRAME_TEST_V` or `XIT_FRAME_GLOBAL_V`.
They do not use loader lifecycle (`initialize`, `update`, `finalize`) and are listed per tag via `frame_info.views`.

More: [Loaders](./05-LOADERS.md), [Tests](./03-TESTS.md), [Macros](./01-MACROS.md), [Binding](./04-BINDING.md).

Shorthand in suites:
```
struct S {
	XIT_VIEWS("marker");
	XIT_INPUTS("cfg");
	XIT_OUTPUTS("result");
	XIT_EXPECTS("expected");
};
```

