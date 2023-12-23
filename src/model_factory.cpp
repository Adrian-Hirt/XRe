#include <xre/model_factory.h>

ModelFactory::ModelFactory() {};

ModelFactory::ModelFactory(ID3D11Device *device, ID3D11DeviceContext *device_context) {
  this->device = device;
  this->device_context = device_context;
};

Model ModelFactory::createCube(DirectX::XMFLOAT4 color) {
  std::vector<vertex> cube_vertices {
    { DirectX::XMFLOAT3(-1.0f, -1.0f, 1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3( 1.0f, -1.0f, 1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f),  color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3( 1.0f, 1.0f, 1.0f),  color, DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

    { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3( 1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3( 1.0f,  1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

    { DirectX::XMFLOAT3(-1.0f, 1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3(-1.0f, 1.0f,  1.0f), color, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3( 1.0f, 1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3( 1.0f, 1.0f,  1.0f), color, DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },

    { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3( 1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), color, DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3( 1.0f, -1.0f,  1.0f), color, DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },

    { DirectX::XMFLOAT3(1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3(1.0f,  1.0f, -1.0f), color, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3(1.0f, -1.0f,  1.0f), color, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3(1.0f,  1.0f,  1.0f), color, DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) },

    { DirectX::XMFLOAT3(-1.0f, -1.0f, -1.0f), color, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3(-1.0f, -1.0f,  1.0f), color, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3(-1.0f,  1.0f, -1.0f), color, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3(-1.0f,  1.0f,  1.0f), color, DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f),  DirectX::XMFLOAT2(1.0f, 1.0f) }
  };

  std::vector<unsigned int> cube_indices{
    2, 1, 0,    // side 1
    3, 1, 2,
    6, 5, 4,    // side 2
    7, 5, 6,
    10, 9, 8,    // side 3
    11, 9, 10,
    14, 13, 12,    // side 4
    15, 13, 14,
    18, 17, 16,    // side 5
    19, 17, 18,
    20, 22, 23,    // side 6
    23, 21, 20
  };

  Mesh cube_mesh = Mesh(this->device, this->device_context, cube_vertices, cube_indices);
  return Model(this->device, this->device_context, { cube_mesh });
};

Model ModelFactory::createGround(float extent) {
  std::vector<vertex> vertices = {
    {  DirectX::XMFLOAT3(-extent, 0.0f, -extent), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    {  DirectX::XMFLOAT3(-extent, 0.0f,  extent), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
    {  DirectX::XMFLOAT3( extent, 0.0f,  extent), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
    {  DirectX::XMFLOAT3( extent, 0.0f, -extent), DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f),  DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) }
  };

  std::vector<unsigned int> indices = {0, 2, 1, 0, 3, 2};

  Mesh ground_mesh = Mesh(this->device, this->device_context, vertices, indices, DATA_FOLDER "/textures/wood.jpg");
  return Model(this->device, this->device_context, { ground_mesh });
};
