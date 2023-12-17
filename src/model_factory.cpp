#include <xre/model_factory.h>

ModelFactory::ModelFactory() {};

ModelFactory::ModelFactory(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  this->device = device;
  this->device_context = device_context;
};

Model ModelFactory::createCube(DirectX::XMFLOAT4 color) {
  std::vector<vertex> cube_vertices {
    {-1.0f, -1.0f,  1.0f, color},
    { 1.0f, -1.0f,  1.0f, color},
    {-1.0f,  1.0f,  1.0f, color},
    { 1.0f,  1.0f,  1.0f, color},
    {-1.0f, -1.0f, -1.0f, color},
    {-1.0f,  1.0f, -1.0f, color},
    { 1.0f, -1.0f, -1.0f, color},
    { 1.0f,  1.0f, -1.0f, color},
  };

  std::vector<unsigned int> cube_indices{
    2, 1, 0,    // side 1
    3, 1, 2,
    6, 5, 4,    // side 2
    7, 5, 6,
    7, 2, 5,    // side 3
    3, 2, 7,
    0, 6, 4,    // side 4
    1, 6, 0,
    1, 7, 6,    // side 5
    3, 7, 1,
    4, 5, 2,    // side 6
    2, 0, 4
  };

  Mesh cube_mesh = Mesh(this->device, this->device_context, cube_vertices, cube_indices);
  return Model(this->device, this->device_context, { cube_mesh });
};

Model ModelFactory::createGround(float extent) {
  std::vector<vertex> vertices = {
    { -extent, 0.0f, -extent, DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)},
    { -extent, 0.0f,  extent, DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)},
    {  extent, 0.0f,  extent, DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)},
    {  extent, 0.0f, -extent, DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f)}
  };

  std::vector<unsigned int> indices = {0, 2, 1, 0, 3, 2};

  Mesh ground_mesh = Mesh(this->device, this->device_context, vertices, indices);
  return Model(this->device, this->device_context, { ground_mesh });
};
