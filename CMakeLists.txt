cmake_minimum_required(VERSION 3.0.0)
project(XRe)

# TODO: will need to make this work with ARM64 later.
set(TARGET_ARCHITECTURE x64)

# TODO: remove and fix warnings
add_compile_options(-Wno-ignored-attributes)

# Enable Action bindings other than khr/simple_controller. Comment this out to only use
# khr/simple_controller for the Mixed Reality-Portal debugging.
# add_compile_definitions(ENABLE_ACTION_BINDINGS)

find_library(OPENXR NAMES openxr_loader PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib/open_xr/${TARGET_ARCHITECTURE}/${CMAKE_BUILD_TYPE})
message(STATUS "***** find_library returned ${OPENXR} *****")
message(STATUS "***** CMake Build type: ${CMAKE_BUILD_TYPE} *****")

FILE(GLOB SRCFILES src/*)

SET(ADDITIONAL_SRCFILES
  "ext/DirectXTex/WICTextureLoader11.cpp"
)

add_executable(${PROJECT_NAME} ${SRCFILES} ${ADDITIONAL_SRCFILES} main.cpp)

set(SHADERS_FOLDER "\"${CMAKE_CURRENT_SOURCE_DIR}/shaders\"" STRING "")
set(DATA_FOLDER "\"${CMAKE_CURRENT_SOURCE_DIR}/data\"" STRING "")
target_compile_definitions(${PROJECT_NAME} PUBLIC SHADERS_FOLDER=${SHADERS_FOLDER} DATA_FOLDER=${DATA_FOLDER})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/ext)
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${OPENXR}
  dxgi
  D3D11
  D3dcompiler
)
