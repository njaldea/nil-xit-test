# CMake helpers for nil-xit-test integration

# Global configuration (cached)
# These control the GUI invocation for all xit tests.
set(NIL_XIT_CLEAR OFF   CACHE BOOL   "Clear on GUI start (-c)")
set(NIL_XIT_PORT "1101" CACHE STRING "Port for XIT GUI server (-p)")
set(NIL_XIT_JOBS "1"    CACHE STRING "Parallel jobs for test execution (-jN)")
set(NIL_XIT_ASSETS ""    CACHE PATH   "Path to GUI assets directory (-a)")

# Create a test executable for xit-test with CTest integration and GUI target
# Usage:
#   add_xit_test(
#       my_test_exe 
#       SOURCES
#           main.cpp
#           other.cpp
#       GROUPS
#           "ui=./frontend"
#           "data=./test_data"
#   )
#   target_link_libraries(my_test_exe PRIVATE nil::xit-gtest)
function(add_xit_test TARGET)
    cmake_parse_arguments(
        XIT
        ""  # No boolean flags
        ""   # No single value args
        "SOURCES;GROUPS"  # Multi-value args
        ${ARGN}
    )

    # Create the test executable directly
    add_executable(${TARGET} ${XIT_SOURCES})
    
    # Apply clang-tidy settings if enabled
    if(ENABLE_CLANG_TIDY AND CMAKE_CXX_CLANG_TIDY)
        set_target_properties(${TARGET} PROPERTIES CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY};-checks=-readability-function-cognitive-complexity")
    endif()
    
    # Create CTest entry with group arguments if provided
    set(group_args "")
    foreach(group IN LISTS XIT_GROUPS)
        list(APPEND group_args "-g" "${group}")
    endforeach()
    
    # Add test with group arguments for CTest
    add_test(NAME ${TARGET} COMMAND ${TARGET} ${group_args})

    # Create GUI launch target
    add_custom_target(${TARGET}_hl
        COMMAND $<TARGET_FILE:${TARGET}> ${group_args}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Launching ${TARGET} in Headless mode"
        USES_TERMINAL
    )

    set(
        gui_args gui
        ${group_args}
        "-p${NIL_XIT_PORT}"
        "-j${NIL_XIT_JOBS}"
        $<$<BOOL:${NIL_XIT_ASSETS}>:-a${NIL_XIT_ASSETS}>
        $<$<BOOL:${NIL_XIT_CLEAR}>:-c>
    )

    # Create GUI launch target
    add_custom_target(${TARGET}_gui
        COMMAND $<TARGET_FILE:${TARGET}> ${gui_args}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Launching ${TARGET} in GUI mode"
        USES_TERMINAL
    )
endfunction()
