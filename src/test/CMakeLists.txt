project(xit-test)

find_package(nil-xit CONFIG REQUIRED)
find_package(nil-gate CONFIG REQUIRED)

set(
    SOURCES
        src/App.cpp
        src/RerunTag.cpp
        src/SingleFire.cpp
)

set(
    HEADERS
        publish/nil/xit/test.hpp
        publish/nil/xit/test/App.hpp
        publish/nil/xit/test/RerunTag.hpp
        publish/nil/xit/test/SingleFire.hpp
        publish/nil/xit/test/utils.hpp
        publish/nil/xit/test/frame/input/Info.hpp
        publish/nil/xit/test/frame/input/Global.hpp
        publish/nil/xit/test/frame/input/Test.hpp
        publish/nil/xit/test/frame/output/Info.hpp
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} PUBLIC nil::xit)
target_link_libraries(${PROJECT_NAME} PUBLIC nil::gate)

nil_install_headers(${PROJECT_NAME} PUBLIC)
nil_install_targets(${PROJECT_NAME})