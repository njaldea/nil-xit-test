cmake_minimum_required(VERSION 3.27.1)

project(nil-xit-test CXX)

include(cmake/package.cmake)
include(cmake/toolchain.cmake)
include(cmake/quality.cmake)
include(cmake/test.cmake)
include(cmake/coverage.cmake)
include(cmake/nil-xit-gtest.cmake)

add_subdirectory(src)
add_subdirectory(sandbox)
add_subdirectory(doc)