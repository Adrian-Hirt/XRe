cmake_minimum_required(VERSION 3.20)
project(XRe)

# TODO: will need to make this work with ARM64 later.
set(TARGET_ARCHITECTURE x64)

# TODO: remove and fix warnings
add_compile_options(-Wno-ignored-attributes)

set(CMAKE_CXX_STANDARD 20)

# Enable Action bindings other than khr/simple_controller. Comment this out to only use
# khr/simple_controller for the Mixed Reality-Portal debugging.
# add_compile_definitions(ENABLE_ACTION_BINDINGS)

find_library(OPENXR NAMES openxr_loader PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/open_xr/${TARGET_ARCHITECTURE}/${CMAKE_BUILD_TYPE})
message(STATUS "***** find_library returned ${OPENXR} *****")
message(STATUS "***** CMake Build type: ${CMAKE_BUILD_TYPE} *****")

# FILE(GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
SET(SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp)

# Needed to access the include folders from the individual apps
SET(XRE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/ext)

# SET(ADDITIONAL_SRCFILES
#   "${CMAKE_CURRENT_LIST_DIR}/ext/DirectXTex/WICTextureLoader11.cpp"
# )

set(SHADERS_FOLDER "\"${CMAKE_CURRENT_SOURCE_DIR}/shaders\"" STRING "")
set(DATA_FOLDER "\"${CMAKE_CURRENT_SOURCE_DIR}/data\"" STRING "")

add_subdirectory(apps)
