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
  output.position = mul(mul(position, model), view_projection);

  // Compute color based on normal vectors
  output.color = ambient_color;
  float4 norm = normalize(mul(rotation, normal));
	float diffuse_brightness = saturate(dot(norm, light_vector));
  float4 resulting_light = light_color * diffuse_brightness;
  resulting_light.a = 1.0f;
	output.color += saturate(resulting_light * color);

  return output;
}

float4 PShader(float4 position : SV_POSITION, float4 color : COLOR) : SV_TARGET {
  return color;
}