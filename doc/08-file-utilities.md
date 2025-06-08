# File Utilities

The library provides the following utility methods to be able to read from a file and write to a file.

> **Related documentation:**
> - [Custom Types](./06-custom-types.md) - For creating serializable custom types
> - [Migration Guide](./02-migration.md#step-5-save-input-to-file) - For examples of using these utilities
> - [Concepts: Data Flow](./03-concepts.md#data-flow) - For understanding how file persistence fits into the framework

- `nil::xit::gtest::from_file<T, literal>`
- `nil::xit::gtest::from_file_with_finalize<T, literal>`
- `nil::xit::gtest::from_file_with_update<T, literal>`

The utility classes provides the following functions:

- `T initialize()` / `T initialize(std::string_view)`
    - for reading the file
- `T finalize()` / `T finalize(std::string_view)`
    - for writing the data
    - used when the finalize hook is triggered by the frontend
- `T update()` / `T update(std::string_view)`
    - for writing the data
    - used every time the frontend changes the value

By default, the utility methods will use `buffer_type` specialization for the types.

If file format has to differ from `buffer_type`, a specialization to `file_codec` can be used.

```cpp
template <>
struct nil::xit::gtest::file_codec<Circle>
{
    static void write(std::ostream& oss, const Circle& data)
    {
        oss << data.x << ',' << data.y << ',' << data.radius;
    }

    static Circle read(std::istream& iss)
    {
        Circle data = {};
        char c = {};
        iss >> data.x >> c >> data.y >> c >> data.radius;
        return data;
    }
};
```