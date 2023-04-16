cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME
    main.out
    )

set(SRCS
    main.cpp
    )

project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)

include_directories(${PROCPS_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SRCS})
