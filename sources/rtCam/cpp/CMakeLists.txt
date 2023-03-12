cmake_minimum_required(VERSION 3.0)

set(MAIN_PROJECT
    main.out
    )

# Définir les fichiers source et header
set(SOURCES
    sources/config.cpp
    sources/main.cpp
)

set(HEADERS
    headers/config.hpp
    headers/main.hpp
)

# Définir la version utilisée de C++
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

project(rtCam)

# Trouver la bibliothèque OpenCV
find_package(OpenCV REQUIRED)

# MAIN PROJECT

# Ajouter les fichiers source et header à la cible exécutable
add_executable(${MAIN_PROJECT} ${SOURCES} ${HEADERS})

# Inclure les répertoires de header
include_directories(${OpenCV_INCLUDE_DIRS})

target_include_directories(${MAIN_PROJECT} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/headers)
target_include_directories(${MAIN_PROJECT} PRIVATE /usr/include/yaml-cpp)

# Lier les bibliothèques nécessaires
target_link_libraries(${MAIN_PROJECT} PRIVATE ${OpenCV_LIBS})
target_link_libraries(${MAIN_PROJECT} PRIVATE yaml-cpp)


# WAITKEY PROJECT

set(WAITKEY_PROJECT
    testWaitKey.out
    )

add_executable(${WAITKEY_PROJECT} sources/testWaitKey.cpp)
target_link_libraries(${WAITKEY_PROJECT} PRIVATE ${OpenCV_LIBS})
