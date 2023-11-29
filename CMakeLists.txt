cmake_minimum_required(VERSION 3.10)

project (Webserver)

set(CMAKE_CXX_STANDARD 20)

add_executable(server src/main.cpp src/response.cpp src/utils/splitstring.cpp)
