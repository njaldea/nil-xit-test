project(xit-test)

find_package(nil-xit CONFIG REQUIRED)
find_package(nil-gate CONFIG REQUIRED)

set(
    SOURCES
        src/App.cpp
        src/RerunTag.cpp
        src/transparent_hash_map.cpp
)

set(
    HEADERS
        publish/nil/xit/test.hpp
        publish/nil/xit/test/App.hpp
        publish/nil/xit/test/RerunTag.hpp
        publish/nil/xit/test/transparent_hash_map.hpp
        publish/nil/xit/test/utils.hpp
        publish/nil/xit/test/frame/input/Info.hpp
        publish/nil/xit/test/frame/input/Unique.hpp
        publish/nil/xit/test/frame/input/Tagged.hpp
        publish/nil/xit/test/frame/output/Info.hpp
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PUBLIC nil::xit)
target_link_libraries(${PROJECT_NAME} PUBLIC nil::gate)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/publish>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

nil_install_headers()
nil_install_targets(${PROJECT_NAME})