#version 450 core
#include "_common.vert"

void main() {
  // Transform vertex position to world space
  vec4 pos = modelUBO.world * vec4(inPosition, 1.0);

  // Transform to clip space
  gl_Position = globalUBO.view_projection * pos;

  // Set color to color of the model
  color = modelUBO.color;

  // Set fragment texture coordinates
  fragTexCoord = inTexCoord;
}
