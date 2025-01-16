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
        table/table.cpp
        database/database.cpp
        enums/columntype.h
        parser/parser.cpp
        parser/parser.h
        enums/constraint.h
        constraintchecker/constraintchecker.cpp
        constraintchecker/constraintchecker.h
        parser/queries/databaseparser.cpp
        parser/queries/databaseparser.h
        parser/queries/select/selectparser.cpp
        parser/queries/select/selectparser.h
        parser/queries/insertparser.cpp
        parser/queries/insertparser.h
        parser/queries/alterparser.cpp
        parser/queries/alterparser.h
        parser/queries/tableparser.cpp
        parser/queries/tableparser.h
        parser/queries/updateparser.cpp
        parser/queries/updateparser.h
        parser/queries/deleteparser.cpp
        parser/queries/deleteparser.h
        enums/jointype.h
        parser/queries/select/whereclauseparser.cpp
        parser/queries/select/whereclauseparser.h
        serializer/serializer.cpp
        serializer/serializer.h
)
target_link_libraries(dbee fmt)
