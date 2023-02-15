cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME
    main.out
    )

set(SRCS
    main.cpp
    )

project(${PROJECT_NAME})
find_package( OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
