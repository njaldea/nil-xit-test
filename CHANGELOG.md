# Changelog

All notable changes to this project will be documented in this file.

The format is based on Keep a Changelog, and this project adheres to Semantic Versioning.

## [v0.0.14] - 2025-12-02
### Fixed
- unique frame file path

## [v0.0.13] - 2025-12-02
### Changed
- BREAKING!!!
- updated nil-xit dependency (v0.3.5)


## [v0.0.12] - 2025-11-22
### Changed
- updated vcpkg baseline dependency

### Fix
- fix cpack dependencies

## [v0.0.11] - 2025-11-1
### Added
- added cpack

## [v0.0.10] - 2025-10-20
### Fixed
- default frame type eq check set to false. if not provided, change will not cause rerun

### Changed
- updated vcpkg baseline dependency

## [v0.0.9] - 2025-10-19
### Changed
- updated dependencies nil-gate(v1.5.1)

## [v0.0.8] - 2025-08-23
### Added
- `-j` support for parallel test execution.
- NIL_XIT_PORT NIL_XIT_JOBS NIL_XIT_CLEAR to add_xit_test
- TestTracker per-thread helper for checking test result
- GTest Override to inspect test result
- added tag_info for test result

### Fixed
- Fixed main signature to allow acceptance of char const * const * and any of its variants
- Default value binding for built-in types

### Changed
- Updated dependencies and test runner main to capture argv properly.
- nil-gate v1.4.0 dep update

## [v0.0.7] - 2025-08-11
### Added
- Added expect frame
- Dedicated docs section for `XIT_INPUTS`, `XIT_OUTPUTS`, and `XIT_EXPECTS` (doc/01-MACROS.md).
- Formatting cleanup across doc examples (doc/test/step_*.cpp).

### Changed
- Removed main converter
- Simplified from_file helpers
- Updated docs
- Revamped Fixture declaration
- Examples migrated to use `XIT_INPUTS`/`XIT_OUTPUTS`/`XIT_EXPECTS` instead of `using input_frames/...`.

## [v0.0.6] - 2025-07-08
### Fixed
- [xit-gtest] finalize for global frame_setup.

## [v0.0.5] - 2025-06-30
### Fixed
- [xit-gtest] output macro bug.

### Changed
- [xit-test] version bump v0.0.5.

## [v0.0.4] - 2025-06-22
### Added
- Allow unrelated data binding for input frames.

## [v0.0.3] - 2025-06-15
### Changed
- Updated gate to allow operator== bypass.
- Updated deps: @nil-/xit v0.4.6 and nil-xit v0.3.1.
- Added installation README.
- Introduced CMake helper: `add_xit_test` and `add_xit_test_executable`.
- Auto download of @nil-/xit 0.4.0.
- Project name simplification: nil-xit-test -> xit-test.
- Updated docs.

## [v0.0.2] - 2025-06-08
### Added
- README.

### Changed
- Updated dependencies.
- Renamed frame names.
- Introduced `file_codec`.

## [v0.0.1] - 2025-06-04
### Added
- Initial tagged release: grouped files setup.
