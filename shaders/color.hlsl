#include "_common.hlsl"

VOut VShader(VIn input) {
  VOut output;

  // Calculate the position
  output.position = mul(mul(input.position, model), view_projection);

  // Simply use the model color for the color
	output.color = model_color;

  // No texture coordinates
  output.texcoord = null_texture_coords;


  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
  return color;
}