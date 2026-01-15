project(xit-gtest)

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
    publish/nil/xit/gtest/headless/CacheManager.hpp
    publish/nil/xit/gtest/builders/install.hpp
    publish/nil/xit/gtest/builders/FrameBuilder.hpp
    publish/nil/xit/gtest/builders/MainBuilder.hpp
    publish/nil/xit/gtest/builders/TestBuilder.hpp
    publish/nil/xit/gtest/builders/frames/IFrame.hpp
    publish/nil/xit/gtest/builders/frames/main/Frame.hpp
    publish/nil/xit/gtest/builders/frames/output/Frame.hpp
    publish/nil/xit/gtest/builders/frames/input/Frame.hpp
    publish/nil/xit/gtest/builders/frames/input/global.hpp
    publish/nil/xit/gtest/builders/frames/input/test.hpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC xit-test)
target_link_libraries(${PROJECT_NAME} PUBLIC GTest::gtest)
target_link_libraries(${PROJECT_NAME} PRIVATE nil::clix)

nil_install_headers(${PROJECT_NAME} PUBLIC)
nil_install_targets(${PROJECT_NAME})