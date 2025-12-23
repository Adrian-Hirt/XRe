function(add_xre_app APP_NAME SRCFILES)
    add_executable(${APP_NAME} ${SRCFILES} main.cpp)

    target_compile_definitions(${APP_NAME} PUBLIC SHADERS_FOLDER=${SHADERS_FOLDER} DATA_FOLDER=${DATA_FOLDER})
    target_include_directories(${APP_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS} ${XRE_INCLUDES})

    target_link_libraries(${APP_NAME} PUBLIC
        ${OPENXR}
        ${Vulkan_LIBRARIES}
    )
endfunction()
