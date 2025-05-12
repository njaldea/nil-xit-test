project(xit-gtest)

find_package(nil-clix CONFIG REQUIRED)
find_package(nil-service CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_library(
    ${PROJECT_NAME}
    src/main.cpp
    src/gtest.cpp
    publish/nil/xit/gtest.hpp
    publish/nil/xit/gtest/Data.hpp
    publish/nil/xit/gtest/Instances.hpp
    publish/nil/xit/gtest/MACROS.hpp
    publish/nil/xit/gtest/main.hpp
    publish/nil/xit/gtest/Test.hpp
    publish/nil/xit/gtest/utils/from_self.hpp
    publish/nil/xit/gtest/utils/from_file.hpp
    publish/nil/xit/gtest/utils/from_data.hpp
    publish/nil/xit/gtest/utils/from_member.hpp
    publish/nil/xit/gtest/headless/Inputs.hpp
    publish/nil/xit/gtest/builders/install.hpp
    publish/nil/xit/gtest/builders/FrameBuilder.hpp
    publish/nil/xit/gtest/builders/MainBuilder.hpp
    publish/nil/xit/gtest/builders/TestBuilder.hpp
    publish/nil/xit/gtest/builders/frames/IFrame.hpp
    publish/nil/xit/gtest/builders/frames/main/Frame.hpp
    publish/nil/xit/gtest/builders/frames/output/Frame.hpp
    publish/nil/xit/gtest/builders/frames/input/Frame.hpp
    publish/nil/xit/gtest/builders/frames/input/unique.hpp
    publish/nil/xit/gtest/builders/frames/input/tagged.hpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC xit-test)
target_link_libraries(${PROJECT_NAME} PRIVATE nil::clix)
target_link_libraries(${PROJECT_NAME} PRIVATE nil::service)
target_link_libraries(${PROJECT_NAME} PRIVATE GTest::gtest)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/publish>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

nil_install_headers()
nil_install_targets(${PROJECT_NAME})