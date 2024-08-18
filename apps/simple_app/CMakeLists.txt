cmake_minimum_required(VERSION 3.0.0)

project(simple_app)

add_executable(${PROJECT_NAME} ${SRCFILES} ${ADDITIONAL_SRCFILES} main.cpp)

target_compile_definitions(${PROJECT_NAME} PUBLIC SHADERS_FOLDER=${SHADERS_FOLDER} DATA_FOLDER=${DATA_FOLDER})
target_include_directories(${PROJECT_NAME} PUBLIC ${XRE_INCLUDES})
target_link_libraries(${PROJECT_NAME} PUBLIC
  ${OPENXR}
  dxgi
  D3D11
  D3dcompiler
)
