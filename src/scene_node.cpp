#include <xre/scene_node.h>

SceneNode::SceneNode() {
  m_parent = NULL;
  m_model = NULL;
  m_shader = Shader::loadOrCreate(SHADERS_FOLDER "/ambient.hlsl");
}

SceneNode::SceneNode(Model* model) {
  m_model = model;
  m_parent = NULL;
  m_shader = Shader::loadOrCreate(SHADERS_FOLDER "/ambient.hlsl");
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
  // TODO: remove `m_shader` param when models have a default shader
  if (m_model) {
    // Update the shader with the transform
    m_shader.setModelMatrix(m_world_transform);
    m_shader.setNormalRotationMatrix(m_world_rotation_matrix);
    // m_shader.setModelColor(m_model->getColor());
    m_shader.updatePerModelConstantBuffer();
    m_model->render(m_shader);
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
