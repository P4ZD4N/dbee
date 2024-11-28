cmake_minimum_required(VERSION 3.29)
project(dbee)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 11.0.2
)

FetchContent_MakeAvailable(fmt)

add_executable(dbee main.cpp
        table.cpp
        database.cpp
        enums/columntype.h
        parser.cpp
        parser.h
        enums/constraint.h
        constraintchecker.cpp
        constraintchecker.h)
target_link_libraries(dbee fmt)
