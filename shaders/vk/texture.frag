#version 450 core

layout(set = 1, binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 color;
layout(location = 1) in vec3 position;
layout(location = 2) in vec2 fragmentCoordinate;

layout(location = 0) out vec4 outColor;

void main() {
  outColor = vec4(texture(texSampler, fragmentCoordinate).rgb, 1.0f);
}
