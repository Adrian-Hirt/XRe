#include <xre/scene_node.h>

SceneNode::SceneNode() {
  m_parent = NULL;
  m_model = NULL;
  m_shader = Shader::loadOrCreate(SHADERS_FOLDER "/color.hlsl");
}

SceneNode::SceneNode(Model* model) {
  m_model = model;
  m_parent = NULL;
  m_shader = Shader::loadOrCreate(SHADERS_FOLDER "/color.hlsl");
}

SceneNode::~SceneNode() {
 m_children.clear();
}

void SceneNode::addChildNode(SceneNode &child) {
  m_children.push_back(&child);
  child.m_parent = this;
}

void SceneNode::render() {
  m_shader.activate();

  if (m_model) {
    // Update the shader with the transform
    m_shader.setModelMatrix(m_world_transform);
    m_shader.setNormalRotationMatrix(m_world_rotation_matrix);
    m_shader.setModelColor(m_model->getColor());
    m_shader.updatePerModelConstantBuffer();
    m_model->renderInSceneNode();
  }

  for (SceneNode *child : m_children) {
    child->render();
  }
}

void SceneNode::updateTransformation() {
  // Update the local transform
  m_local_transform = DirectX::XMMatrixTranspose(
    DirectX::XMMatrixAffineTransformation(
      m_scaling,
      DirectX::g_XMZero,
      m_rotation,
      m_translation
    )
  );

  if (m_parent) {
    m_world_transform = DirectX::XMMatrixMultiply(
      m_parent->m_world_transform,
      m_local_transform
    );

    m_world_rotation_matrix = DirectX::XMMatrixRotationQuaternion(
      DirectX::XMQuaternionMultiply(m_parent->m_rotation, m_rotation)
    );
  }
  else {
    // No parent, is the root node, which means its local transform
    // is also its world transform, since the local transform is applied
    // relative to the origin point.
    m_world_transform = m_local_transform;
    m_world_rotation_matrix = DirectX::XMMatrixRotationQuaternion(m_rotation);
  }

  for (SceneNode *child : m_children) {
    child->updateTransformation();
  }
}

void SceneNode::rotate(float roll, float pitch, float yaw) {
  DirectX::XMVECTOR rotation = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
  rotate(rotation);
}

void SceneNode::rotate(DirectX::XMVECTOR rotation) {
  m_rotation = DirectX::XMQuaternionMultiply(m_rotation, rotation);
}

void SceneNode::translate(float x, float y, float z) {
  DirectX::XMVECTOR translation = DirectX::XMVECTORF32({x, y, z});
  translate(translation);
}

void SceneNode::translate(DirectX::XMVECTOR translation) {
  m_translation = DirectX::XMVectorAdd(m_translation, translation);
}

void SceneNode::scale(float x, float y, float z) {
  DirectX::XMVECTOR scaling = DirectX::XMVECTORF32({x, y, z});
  scale(scaling);
}

void SceneNode::scale(DirectX::XMVECTOR scaling) {
  m_scaling = DirectX::XMVectorMultiply(m_scaling, scaling);
}

void SceneNode::setRotation(DirectX::XMVECTOR rotation) {
  m_rotation = rotation;
}

void SceneNode::setScale(float x, float y, float z) {
  DirectX::XMVECTOR scaling = DirectX::XMVECTORF32({x, y, z});
  setScale(scaling);
}

void SceneNode::setScale(DirectX::XMVECTOR scaling) {
  m_scaling = scaling;
}

void SceneNode::setPosition(float x, float y, float z) {
  DirectX::XMVECTOR position = DirectX::XMVECTORF32({x, y, z});
  setPosition(position);
}

void SceneNode::setPosition(DirectX::XMVECTOR position) {
  m_translation = position;
}
