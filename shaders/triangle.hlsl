struct VOut {
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

cbuffer ConstantBuffer{
  float4x4 model;
  float4x4 view_projection;
  float4x4 rotation;
  float4 mesh_color;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR) {
  VOut output;

  // Calculate the position
  output.position = mul(mul(position, model), view_projection);

  // TODO: compute color based on normal vectors
  output.color = color;

  return output;
}


float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
  return color;
}