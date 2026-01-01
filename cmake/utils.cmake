function(add_xre_app APP_NAME SRCFILES)
    file(GLOB SCENE_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/scenes/*.cpp
    )

    add_executable(${APP_NAME}
        main.cpp
        ${SRCFILES}
        ${SCENE_SOURCES}
    )

    target_compile_definitions(${APP_NAME} PUBLIC SHADERS_FOLDER=${SHADERS_FOLDER} DATA_FOLDER=${DATA_FOLDER})
    target_include_directories(${APP_NAME} PUBLIC
        ${Vulkan_INCLUDE_DIRS}
        ${XRE_INCLUDES}
        ${CMAKE_CURRENT_SOURCE_DIR}/scenes
    )

    target_link_libraries(${APP_NAME} PUBLIC
        ${OPENXR}
        ${Vulkan_LIBRARIES}
    )
endfunction()
