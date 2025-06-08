add_library(SUT INTERFACE Circle.hpp)

add_test_executable(step_1 step_1.cpp)

target_link_libraries(step_1 PRIVATE SUT)
target_link_libraries(step_1 PRIVATE GTest::gtest_main)

add_test_executable(step_2 step_2.cpp)

target_link_libraries(step_2 PRIVATE SUT)
target_link_libraries(step_2 PRIVATE xit-gtest)

add_test_executable(step_3 step_3.cpp)

target_link_libraries(step_3 PRIVATE SUT)
target_link_libraries(step_3 PRIVATE xit-gtest)

add_test_executable(step_4 step_4.cpp)

target_link_libraries(step_4 PRIVATE SUT)
target_link_libraries(step_4 PRIVATE xit-gtest)

add_test_executable(step_5 step_5.cpp)

target_link_libraries(step_5 PRIVATE SUT)
target_link_libraries(step_5 PRIVATE xit-gtest)