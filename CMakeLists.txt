cmake_minimum_required(VERSION 3.27.1)

project(
    nil-xit-test
    VERSION 0.0.15
    LANGUAGES CXX
    DESCRIPTION "Test layer to nil-xit"
)


install(
    FILES cmake/nil-xit-gtest.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${CMAKE_PROJECT_NAME}
)

find_package(nil-clix CONFIG REQUIRED)
find_package(nil-xit CONFIG REQUIRED)
find_package(nil-gate CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
list(APPEND CPACK_DEBIAN_PACKAGE_DEPENDS "nil-clix-dev (>= ${nil-clix_VERSION})")
list(APPEND CPACK_DEBIAN_PACKAGE_DEPENDS "nil-xit-dev (>= ${nil-xit_VERSION})")
list(APPEND CPACK_DEBIAN_PACKAGE_DEPENDS "nil-gate-dev (>= ${nil-gate_VERSION})")
list(APPEND CPACK_DEBIAN_PACKAGE_DEPENDS "libgtest-dev (>= ${GTest_VERSION})")
string(JOIN ", " CPACK_DEBIAN_PACKAGE_DEPENDS ${CPACK_DEBIAN_PACKAGE_DEPENDS})

find_package(nil-cmake CONFIG REQUIRED)
install_config("cmake/config.cmake.in")
include(cmake/nil-xit-gtest.cmake)

add_subdirectory(src)
add_subdirectory(sandbox)
add_subdirectory(doc)