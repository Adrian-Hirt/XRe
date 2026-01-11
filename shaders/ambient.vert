#version 450 core
#include "_common.vert"

void main() {
  // Transform vertex position to world space
  vec4 pos = modelUBO.world * vec4(inPosition, 1.0);

  // Transform to clip space
  gl_Position = perFrameUBO.view_projection * pos;

  // Extract rotation+scale, then fix for non-uniform scaling
  mat3 normalMatrix = transpose(inverse(mat3(modelUBO.world)));
  vec3 norm = normalize(normalMatrix * inNormal);

  // Compute diffuse lighting
  float diffuse = clamp(dot(norm, normalize(perFrameUBO.light_vector)), 0.0, 1.0);
  vec3 diffuseLight = perFrameUBO.light_color * diffuse;

  // Combine with ambient
  color = perFrameUBO.ambient_color + diffuseLight;

  // And tint by general model color
  color *= modelUBO.color;
}
