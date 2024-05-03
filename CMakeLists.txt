cmake_minimum_required(VERSION 3.27)
project(DatabaseProject)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 10.2.1
)

FetchContent_MakeAvailable(fmt)

add_executable(DatabaseProject main.cpp
        engine/table.h
        engine/parser/parser.cpp
        engine/parser/parser.h
        engine/query.h
        engine/parser/parse_error.h
        engine/dbms.h
        engine/executor/executor.h
        engine/executor/execution_error.h
        engine/executor/execution_result.h
        engine/executor/executor.cpp
        engine/executor/executor.cpp
        engine/optimizer/optimizer.cpp
        engine/optimizer/optimizer.h)
target_link_libraries(DatabaseProject fmt)