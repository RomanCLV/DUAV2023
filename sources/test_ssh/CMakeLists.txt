cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME
    main.out
    )

set(SRCS
    main.cpp
    )

project(${PROJECT_NAME} LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PROCPS REQUIRED IMPORTED_TARGET libprocps)
include_directories(${PROCPS_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::PROCPS)
