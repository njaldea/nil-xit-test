project(xit-test)

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
        publish/nil/xit/test/frame/expect/Info.hpp
        publish/nil/xit/test/frame/IDataManager.hpp
        publish/nil/xit/test/frame/output/Info.hpp
        publish/nil/xit/test/frame/view/Info.hpp
        publish/nil/xit/test/frame/view/Global.hpp
        publish/nil/xit/test/frame/view/Test.hpp
)

add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "nil-${PROJECT_NAME}")
target_link_libraries(${PROJECT_NAME} PUBLIC nil::xit)
target_link_libraries(${PROJECT_NAME} PUBLIC nil::gate)

nil_install_headers(${PROJECT_NAME} PUBLIC)
nil_install_targets(${PROJECT_NAME})