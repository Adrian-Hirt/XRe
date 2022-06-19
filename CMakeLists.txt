cmake_minimum_required(VERSION 3.0.0)
project(XRe)

FILE(GLOB SRCFILES src/*)

add_executable(${PROJECT_NAME} ${SRCFILES} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

