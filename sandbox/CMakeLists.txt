project(sandbox)

set(ENABLE_SANDBOX  OFF CACHE BOOL "[0 | OFF - 1 | ON]: build sandbox?")
if(NOT ENABLE_SANDBOX)
    return()
endif()

# Include the nil-xit-test CMake helpers
include(${CMAKE_SOURCE_DIR}/cmake/nil-xit-gtest.cmake)

find_package(nlohmann_json CONFIG REQUIRED)

# Create the test executable with sources and GUI target
add_xit_test_executable(
    ${PROJECT_NAME}
    SOURCES
        main.cpp
        main2.cpp
        frame_setup.hpp
        userland_utils.cpp
        userland_utils.hpp
    GROUPS
        "base=${CMAKE_CURRENT_SOURCE_DIR}/gui"
        "test=${CMAKE_CURRENT_SOURCE_DIR}/files"
)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)