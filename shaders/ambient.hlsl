#include "_common.hlsl"

VOut VShader(VIn input) {
  VOut output;

  // Calculate the position
  output.position = mul(mul(input.position, model), view_projection);

  // Compute color based on normal vectors
  output.color = ambient_color;
  float4 norm = normalize(mul(rotation, input.normal));
	float diffuse_brightness = saturate(dot(norm, light_vector));
  float4 resulting_light = light_color * diffuse_brightness;
  resulting_light.a = 1.0f;
	output.color += saturate(resulting_light * color_white);

  // No texture coordinates
  output.texcoord = null_texture_coords;

  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
  return color;
}