# CMake helpers for nil-xit-test integration

# Create a test executable for nil-xit-test with CTest integration and GUI target
# Usage:
#   add_xit_test_executable(my_test_exe 
#       SOURCES main.cpp other.cpp
#       GROUPS "ui=./frontend" "data=./test_data"
#   )
function(add_xit_test_executable TARGET)
    cmake_parse_arguments(
        XIT
        ""  # No boolean flags
        ""  # No single value args
        "SOURCES;GROUPS"  # Multi-value args
        ${ARGN}
    )
    
    # Create the test executable directly
    add_executable(${TARGET} ${XIT_SOURCES})
    target_link_libraries(${TARGET} PRIVATE xit-gtest)
    
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
    
    # Always create GUI launch target
    set(gui_args gui ${group_args} -a "${CMAKE_CURRENT_SOURCE_DIR}/node_modules/@nil-/xit/assets")
    
    # Convert list to string for echo display
    string(REPLACE ";" " " gui_args_str "${gui_args}")
    
    # Create GUI launch target
    add_custom_target(${TARGET}_gui
        COMMAND ${CMAKE_COMMAND} -E echo "Executing: $<TARGET_FILE:${TARGET}> ${gui_args_str}"
        COMMAND $<TARGET_FILE:${TARGET}> ${gui_args}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Launching ${TARGET} in GUI mode"
        USES_TERMINAL
    )
    
endfunction()
