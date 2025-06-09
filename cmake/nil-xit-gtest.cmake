# CMake helpers for nil-xit-test integration

include(ExternalProject)

set(TARBALL_URL "https://registry.npmjs.org/@nil-/xit/-/xit-0.4.0.tgz")
set(TARBALL_PATH "${CMAKE_BINARY_DIR}/xit-0.4.0.tgz")
set(EXTRACT_DIR "${CMAKE_BINARY_DIR}/assets/xit")

ExternalProject_Add(
    xit_assets_download
    PREFIX ${CMAKE_BINARY_DIR}/xit_dl
    URL ${TARBALL_URL}
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}
    SOURCE_DIR ${CMAKE_BINARY_DIR}/assets/xit
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_BUILD ON
)

# Create a test executable for xit-test with CTest integration and GUI target
# Usage:
#   add_xit_test(my_test_exe 
#       SOURCES main.cpp other.cpp
#       GROUPS "ui=./frontend" "data=./test_data"
#   )
#   target_link_libraries(my_test_exe PRIVATE nil::xit-gtest)
function(add_xit_test TARGET)
    cmake_parse_arguments(
        XIT
        ""  # No boolean flags
        ""  # No single value args
        "SOURCES;GROUPS"  # Multi-value args
        ${ARGN}
    )
    
    # Create the test executable directly
    add_executable(${TARGET} ${XIT_SOURCES})
    
    # Ensure assets are downloaded before building
    add_dependencies(${TARGET} xit_assets_download)
    
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
    set(gui_args gui ${group_args} -a "${EXTRACT_DIR}/assets")
    
    # Convert list to string for echo display
    string(REPLACE ";" " " gui_args_str "${gui_args}")
    
    # Create GUI launch target
    add_custom_target(${TARGET}_gui
        COMMAND $<TARGET_FILE:${TARGET}> ${gui_args}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Launching ${TARGET} in GUI mode"
        USES_TERMINAL
    )
    
    # Ensure GUI target depends on both the test executable and downloaded assets
    add_dependencies(${TARGET}_gui ${TARGET} xit_assets_download)
    
endfunction()
