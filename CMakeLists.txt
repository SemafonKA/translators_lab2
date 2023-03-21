cmake_minimum_required(VERSION 3.0.0)
project(
    lab2_scanner 
    VERSION 0.1.0
    LANGUAGES CXX
)

# include(CTest)
# enable_testing()

add_executable(lab2_scanner lib/main.cpp)

set(CMAKE_CXX_STANDARD 20)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
