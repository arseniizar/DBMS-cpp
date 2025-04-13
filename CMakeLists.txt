cmake_minimum_required(VERSION 3.27)
project(DatabaseProject)

set(CMAKE_CXX_STANDARD 20)
# Something for QT
set(CMAKE_AUTOMOC ON)

include(FetchContent)

set(CMAKE_PREFIX_PATH "D:\\Qt\\6.9.0\\mingw_64\\lib\\cmake\\Qt6")

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 10.2.1
)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

file(GLOB_RECURSE PRODUCTION_SOURCES CONFIGURE_DEPENDS "engine/*.cpp")
add_library(DatabaseLib STATIC ${PRODUCTION_SOURCES}
        engine/ui/ui.cpp
        engine/ui/ui.hpp)
target_include_directories(DatabaseLib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/engine)
target_link_libraries(DatabaseLib PRIVATE fmt)
target_link_libraries(DatabaseLib PRIVATE Qt6::Widgets fmt)

add_executable(DatabaseProject main.cpp)
target_link_libraries(DatabaseProject PRIVATE DatabaseLib Qt6::Widgets fmt)

enable_testing()
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "tests/*.cpp")
add_executable(unit_tests ${TEST_SOURCES})
target_link_libraries(unit_tests PRIVATE DatabaseLib gtest_main fmt)
include(GoogleTest)
gtest_discover_tests(unit_tests)
