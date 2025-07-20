# Frame Types

| Frame Type    | Has Data? | Test writes? | UI edits? |
|:-------------:|:---------:|:------------:|:---------:|
| Main          | No        | N/A          | N/A       |
| Test Input    | Yes       | No           | Yes       |
| Global Input  | Yes       | No           | Yes       |
| Expect        | No        | Yes          | Finalize  |
| Output        | Yes       | Yes          | No        |
| Utility (`_V`)| No        | N/A          | No        |

More: [Loaders](./05-LOADERS.md), [Tests](./03-TESTS.md), [Macros](./01-MACROS.md), [Binding](./04-BINDING.md).

Shorthand in suites:
```
struct S { XIT_INPUTS("cfg"); XIT_OUTPUTS("result"); XIT_EXPECTS("expected"); };
```

