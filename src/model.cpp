#include <xre/model.h>

//------------------------------------------------------------------------------------------------------
// Empty default constructor which we need
//------------------------------------------------------------------------------------------------------
Model::Model() {};

//------------------------------------------------------------------------------------------------------
// Initialize the model.
// Arguments:
//  1) DirectX Device
//  2) DirectX DeviceContext
//  3) Vector of meshes for this model
//------------------------------------------------------------------------------------------------------
Model::Model(ID3D11Device *device, ID3D11DeviceContext *device_context, std::vector<Mesh> meshes) {
  this->device = device;
  this->device_context = device_context;
  this->meshes = meshes;
}

void Model::render() {
  // Get the current active shader
  Shader* shader = Shader::getCurrentActiveShader();

  render(shader);
}

void Model::render(Shader *shader) {
  // Activate the shader
  shader->activate();

  // Update the shader with the model matrix
  shader->setModelMatrix(getTransformationMatrix());
  shader->setNormalRotationMatrix(getRotationMatrix());
  shader->updatePerModelConstantBuffer();

  for (Mesh &mesh : meshes) {
    mesh.render();
  }
}

DirectX::XMMATRIX Model::getTransformationMatrix() {
  return DirectX::XMMatrixTranspose(DirectX::XMMatrixAffineTransformation(
         scaling,
         DirectX::g_XMZero,
         rotation,
         translation));

}

DirectX::XMMATRIX Model::getRotationMatrix() {
  return DirectX::XMMatrixRotationQuaternion(this->rotation);
}

void Model::rotate(float roll, float pitch, float yaw) {
  DirectX::XMVECTOR rotation = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
  rotate(rotation);
};

void Model::rotate(DirectX::XMVECTOR rotation) {
  this->rotation = DirectX::XMQuaternionMultiply(this->rotation, rotation);
};

void Model::translate(float x, float y, float z) {
  DirectX::XMVECTOR translation = DirectX::XMVECTORF32({x, y, z});
  translate(translation);
};

void Model::translate(DirectX::XMVECTOR translation) {
  this->translation = DirectX::XMVectorAdd(this->translation, translation);
};

void Model::scale(float x, float y, float z) {
  DirectX::XMVECTOR scaling = DirectX::XMVECTORF32({x, y, z});
  scale(scaling);
};

void Model::scale(DirectX::XMVECTOR scaling) {
this->scaling = DirectX::XMVectorMultiply(this->scaling, scaling);
};

void Model::setRotation(DirectX::XMVECTOR rotation) {
  this->rotation = rotation;
};

void Model::setScale(float x, float y, float z) {
  DirectX::XMVECTOR scaling = DirectX::XMVECTORF32({x, y, z});
  setPosition(scaling);
};

void Model::setScale(DirectX::XMVECTOR scaling) {
  this->scaling = scaling;
};

void Model::setPosition(float x, float y, float z) {
  DirectX::XMVECTOR position = DirectX::XMVECTORF32({x, y, z});
  setPosition(position);
};

void Model::setPosition(DirectX::XMVECTOR position) {
  this->translation = position;
};
