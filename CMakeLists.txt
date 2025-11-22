cmake_minimum_required(VERSION 3.27.1)

project(nil-xit-test VERSION 0.0.12 LANGUAGES CXX)

include(cmake/package.cmake)
include(cmake/toolchain.cmake)
include(cmake/quality.cmake)
include(cmake/test.cmake)
include(cmake/coverage.cmake)
include(cmake/nil-xit-gtest.cmake)
include(cmake/cpack.cmake)

install(
    FILES cmake/nil-xit-gtest.cmake
    DESTINATION ${CMAKE_INSTALL_DATADIR}/${NIL_INSTALL_GROUP}
)

add_subdirectory(src)
add_subdirectory(sandbox)
add_subdirectory(doc)