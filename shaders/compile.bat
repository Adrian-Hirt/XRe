glslc --target-env=vulkan1.3 -std=450core basic.vert -o basic.vert.spv
glslc --target-env=vulkan1.3 -std=450core basic.frag -o basic.frag.spv
glslc --target-env=vulkan1.3 -std=450core ambient.vert -o ambient.vert.spv
glslc --target-env=vulkan1.3 -std=450core texture.frag -o texture.frag.spv
glslc --target-env=vulkan1.3 -std=450core bitmap.vert -o bitmap.vert.spv
pause
