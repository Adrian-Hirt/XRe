// ViewProjection matrix, which is updated once per rendered frame
// (or rather, once per frame and view, i.e. for stereo views it's
// updated twice per frame).
cbuffer PerFrameConstantBuffer{
  float4x4 view_projection;
};

// Buffer holding the data which is constant for a model, i.e. the
// model transformation matrix, as well as the rotation (which we
// might need to rotate the normals about).
cbuffer PerModelConstantBuffer{
  float4x4 model;
  float4x4 rotation;
  float4 model_color;
};

// Struct holding the data for lighting, which currently is only
// used by the ambient light shader
cbuffer LightContstantBuffer {
  float4 light_vector;
	float4 light_color;
	float4 ambient_color;
};

// Output of the vertex shader
struct VOut {
  float4 position : SV_POSITION;
  float4 color : COLOR;
  float2 texcoord : TEXCOORD;
};

// Input of the vertex shader
struct VIn {
  float4 position : POSITION;
  float4 normal : NORMAL;
  float2 texcoord : TEXCOORD;
};

// Some static colors and coordinates
static float4 color_white = { 1.0f, 1.0f, 1.0f, 1.0f };
static float2 null_texture_coords = { 0.0f, 0.0f };
