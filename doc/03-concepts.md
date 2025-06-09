# Core Concepts

Key concepts and terminology used throughout xit-test.

## Frame

A frame connects your C++ test data to UI visualization components.

### Frame Types

**Input Frames** - Provide data to tests
- **Test Input**: Specific to one test
- **Global Input**: Shared across tests
- Visualize and modify test parameters in the UI
- Save inputs for future test runs

**Output Frames** - Display test results
- Visualize test outcomes
- Show complex data in understandable formats

## Group

Groups organize tests and map to filesystem paths for locating UI components and resources.

**Purpose:**
- **Path Resolution**: Map `$group/path` syntax to filesystem locations
- **Test Organization**: Logically group related tests
- **Resource Sharing**: Share inputs and configurations within a group

**Usage:** Reference groups using `$group/path` syntax (e.g., `$test/data.json`)

## Test Structure

Tests extend GTest with visualization capabilities:

**Components:**
- **Test Case**: C++ code executing test logic
- **Test Fixtures**: Setup, teardown, and shared functionality
- **Test Inputs**: Data from input frames
- **Test Outputs**: Results captured in output frames

**Test Identification:** Tests are identified by:
- Suite name + Test name + Group path + Optional tags

## UI Components

Visual representation files for frames:
- **Input Components**: Display and manipulate input data
- **Output Components**: Visualize test results  
- **Binding**: Connect C++ data to UI elements

## Data Flow

1. Input data loaded from files or provided programmatically
2. Test processes inputs and produces outputs
3. Outputs captured in output frames
4. UI components visualize inputs and outputs
5. Input changes can be persisted for future runs

## Execution Modes

- **Headless Mode**: Traditional GTest with assertions
- **GUI Mode**: Tests with visualization UI enabled