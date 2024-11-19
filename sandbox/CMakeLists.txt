project(sandbox)

set(ENABLE_SANDBOX  OFF CACHE BOOL "[0 | OFF - 1 | ON]: build sandbox?")
if(NOT ENABLE_SANDBOX)
    return()
endif()

find_package(nil-clix CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    userland_utils.cpp
    userland_utils.hpp
    xit_gtest.hpp
    xit_gtest.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE xit-test)
target_link_libraries(${PROJECT_NAME} PRIVATE nil::clix)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)