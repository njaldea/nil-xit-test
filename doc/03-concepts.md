# Core Concepts of nil-xit-gtest

This document explains the key concepts and terminology used throughout the framework.

## Frame

A frame is a set of information that represents data or visualization in the testing framework. Frames serve as the bridge between your C++ tests and the visual UI components.

### Types of Frames

#### Input Frames
Input frames provide test data to your test cases. They can be:
- **Test Input Frames**: Input specific to a particular test
- **Global Input Frames**: Input shared across multiple tests

Input frames allow you to:
- Visualize input data in the UI
- Modify test parameters through the UI
- Save test inputs for future test runs

#### Output Frames
Output frames display the results or state after test execution. They:
- Visualize the test results
- Show complex data structures in a comprehensible way
- Enable visual verification of test outcomes

## Group

Groups are organizational units used for categorizing and locating resources. They serve multiple purposes:

1. **Path Resolution**: Groups map to filesystem paths to locate UI components, test files, and other resources
2. **Test Organization**: Logically group related tests together
3. **Resource Sharing**: Enable sharing of inputs and configurations within a group

Groups are referenced using the `$group/path` syntax throughout the framework, where `$group` represents a defined group name.

## Test Structure

Tests in nil-xit-gtest extend the traditional GTest structure with visualization capabilities:

### Components
- **Test Case**: The actual C++ code that executes test logic
- **Test Fixtures**: Classes that provide setup, teardown, and shared functionality
- **Test Inputs**: Data provided to tests through input frames
- **Test Outputs**: Results captured in output frames

### Test Identification
Tests are identified using a combination of:
- Suite name
- Test name
- Group path
- Optional tags

## UI Components

UI components are files that provide the visual representation of frames:

- **Input Components**: UI elements for displaying and manipulating input data
- **Output Components**: UI elements for visualizing test results
- **Binding**: Connection points between C++ data and UI elements

## File System Structure

The framework relies on specific file system organization:

- **Test Files**: C++ files containing test logic
- **UI Files**: Components for visualization
- **Input Files**: Stored test inputs (JSON, MessagePack, etc.)
- **Group Directories**: Folders corresponding to defined groups

## Data Flow

Understanding data flow in nil-xit-gtest:

1. Input data is loaded from files or provided programmatically
2. Test code processes inputs and produces outputs
3. Outputs are captured in output frames
4. UI components visualize inputs and outputs
5. Changes to inputs can be persisted for future test runs

## Execution Modes

The framework supports multiple execution modes:

- **Headless Mode**: Run tests with assertions like traditional GTest
- **GUI Mode**: Run tests with visualization UI enabled