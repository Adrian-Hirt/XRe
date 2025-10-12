#include "_common.hlsl"

// This shader simply uses the input position and does not apply and model transformations
VOut VShader(VIn input) {
  VOut output;

  // Position is simply the input position multiplied by the view projection
  output.position = mul(input.position, view_projection);

  // Simply use the model color for the color
	output.color = model_color;

  // No texture coordinates
  output.texcoord = null_texture_coords;


  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
  return color;
}