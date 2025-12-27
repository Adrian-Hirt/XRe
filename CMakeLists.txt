cmake_minimum_required(VERSION 3.20)
project(XRe)

# TODO: will need to make this work with ARM64 later.
set(TARGET_ARCHITECTURE x64)

# TODO: remove and fix warnings
add_compile_options(-Wno-ignored-attributes)
add_compile_options(-Wno-narrowing)

set(CMAKE_CXX_STANDARD 20)

# Enable Action bindings other than khr/simple_controller. Comment this out to only use
# khr/simple_controller for the Mixed Reality-Portal debugging.
add_compile_definitions(ENABLE_OCULUS_TOUCH_BINDINGS)

message(STATUS "CMake Build type: ${CMAKE_BUILD_TYPE}")

# Find OpenXR
find_library(OPENXR NAMES openxr_loader PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/open_xr/${TARGET_ARCHITECTURE}/${CMAKE_BUILD_TYPE})
if(OPENXR)
  message(STATUS "OpenXR library found: ${OPENXR}")
else()
  message(FATAL_ERROR "OpenXR library not found")
endif()

# Find Vulkan
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
  message(STATUS "Vulkan found: ${Vulkan_INCLUDE_DIR} / ${Vulkan_LIBRARY}")
else()
  message(FATAL_ERROR "Vulkan not found")
endif()

# FILE(GLOB SRCFILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
SET(SRCFILES
  ${CMAKE_CURRENT_SOURCE_DIR}/src/application.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/openxr_handler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/vulkan_handler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/render_target.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/renderable.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/mesh.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/model.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/scene_node.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/controller.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/hand.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/line.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/object_oriented_bounding_box.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/material.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/resource_manager.cpp
)

# Setup clang format
find_program(CLANG_FORMAT_EXE NAMES clang-format)
if(CLANG_FORMAT_EXE)
  add_custom_target(
    clang_format
    COMMAND ${CLANG_FORMAT_EXE} -i ${CMAKE_CURRENT_SOURCE_DIR}/include/xre/*.h ${SRCFILES}
    COMMENT "Running clang-format on all source files"
  )

  add_custom_target(
    clang_format_check
    COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${CMAKE_CURRENT_SOURCE_DIR}/include/xre/*.h ${SRCFILES}
    COMMENT "Checking clang-format style"
  )
else()
  message(WARNING "clang-format not found! Skipping formatting target.")
endif()

# Needed to access the include folders from the individual apps
SET(XRE_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/ext)

# SET(ADDITIONAL_SRCFILES
#   "${CMAKE_CURRENT_LIST_DIR}/ext/DirectXTex/WICTextureLoader11.cpp"
# )

set(SHADERS_FOLDER "\"${CMAKE_CURRENT_SOURCE_DIR}/shaders/\"" STRING "")
set(DATA_FOLDER "\"${CMAKE_CURRENT_SOURCE_DIR}/data/\"" STRING "")

# Allow using the functions defined in the utils file
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(utils)

add_subdirectory(apps)
