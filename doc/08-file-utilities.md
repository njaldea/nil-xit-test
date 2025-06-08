# File Utilities

Utilities for reading and writing test data to files.

> **Related documentation:**
> - [Custom Types](./06-custom-types.md) - Creating serializable custom types
> - [Migration Guide](./02-migration.md#step-5-save-input-to-file) - Usage examples
> - [Concepts: Data Flow](./03-concepts.md#data-flow) - How file persistence fits the framework

## Utility Classes

- `nil::xit::gtest::from_file<T, literal>`
- `nil::xit::gtest::from_file_with_finalize<T, literal>`
- `nil::xit::gtest::from_file_with_update<T, literal>`

## Available Methods

- `T initialize()` / `T initialize(std::string_view)` - Read from file
- `T finalize()` / `T finalize(std::string_view)` - Write data (triggered by frontend finalize hook)
- `T update()` / `T update(std::string_view)` - Write data (triggered by frontend value changes)

By default, utilities use `buffer_type` specialization for serialization.

## Custom File Format

To use a different file format than `buffer_type`, specialize `file_codec`:

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