struct VOut {
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

cbuffer PerFrameConstantBuffer{
  float4x4 view_projection;
};

cbuffer PerModelConstantBuffer{
  float4x4 model;
  float4x4 rotation;
};

cbuffer LightContstantBuffer {
  float4 light_vector;
	float4 light_color;
	float4 ambient_color;
};

VOut VShader(float4 position : POSITION, float4 color : COLOR, float4 normal : NORMAL) {
  VOut output;

  // Calculate the position
  output.position = position;

  // Simply use the color we got passed in as the color
	output.color = color;

  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
  return color;
}