cmake_minimum_required(VERSION 3.26)
project(learn-stl)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXPORT_COMPILE_COMMANDS ON)
# Add executable for your test cases
add_executable(main
    main.cpp
)
