cmake_minimum_required(VERSION 3.16)

project(BinaryTreesResearch VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Основные исходные файлы
add_executable(BinaryTreesResearch
    src/main.cpp
    src/AVLTree.cpp
    src/AATree.cpp
    src/Treap.cpp
    src/TwoThreeTree.cpp
    src/Benchmark.cpp
    src/DataGenerator.cpp
    src/ConsoleUI.cpp
    src/Statistics.cpp
    src/Visualizer.cpp
    src/FileExporter.cpp
)

# Подключаем заголовочные файлы
target_include_directories(BinaryTreesResearch PUBLIC include)

# Для Windows
if(WIN32)
    target_compile_definitions(BinaryTreesResearch PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()