cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME
    main.out
    )

set(SRCS
    main.cpp
    )

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV 4 REQUIRED)
find_package(Boost REQUIRED COMPONENTS system)

include_directories(${OpenCV_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${Boost_LIBRARIES})
