project(xit-test)

find_package(nil-xit CONFIG REQUIRED)
find_package(nil-gate CONFIG REQUIRED)

set(
    SOURCES
        src/test/App.cpp
        src/test/RerunTag.cpp
        src/test/transparent_hash_map.cpp
)

set(
    HEADERS
        publish/nil/xit/test.hpp
        publish/nil/xit/test/App.hpp
        publish/nil/xit/test/frame/IInfo.hpp
        publish/nil/xit/test/frame/input/Info.hpp
        publish/nil/xit/test/frame/input/Unique.hpp
        publish/nil/xit/test/frame/input/Tagged.hpp
        publish/nil/xit/test/frame/output/Info.hpp
        publish/nil/xit/test/transparent_hash_map.hpp
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PUBLIC nil::xit)
target_link_libraries(${PROJECT_NAME} PUBLIC nil::gate)

target_include_directories(
    ${PROJECT_NAME}
    SYSTEM PRIVATE
    $<TARGET_PROPERTY:Boost::boost,INTERFACE_INCLUDE_DIRECTORIES>
)
# target_compile_definitions(${PROJECT_NAME} PRIVATE BOOST_ASIO_NO_DEPRECATED)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/publish>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

nil_install_headers()
nil_install_targets(${PROJECT_NAME})