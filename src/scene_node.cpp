#include <xre/scene_node.h>

SceneNode::SceneNode() {
  m_parent = NULL;
  m_model = NULL;
}

SceneNode::SceneNode(Model* model) {
  m_model = model;
  buildBoundingBox();
  m_parent = NULL;
}

SceneNode::SceneNode(Model* model, SceneNode &parent) {
  m_model = model;
  buildBoundingBox();
  m_parent = &parent;
  parent.addChildNode(this);
}

SceneNode::SceneNode(Renderable* renderable) {
  m_renderable = renderable;
}

SceneNode::SceneNode(Renderable* renderable, SceneNode &parent) {
  m_renderable = renderable;
  m_parent = &parent;
  parent.addChildNode(this);
}

void SceneNode::buildBoundingBox() {
//   std::vector<DirectX::XMFLOAT3> bounding_box_corners = m_model->getMeshBoundingBoxCorners();
//   size_t points_count = bounding_box_corners.size();

//   // Build the bounding box
//   DirectX::BoundingOrientedBox::CreateFromPoints(m_model_bounding_box, points_count, bounding_box_corners.data(), sizeof(DirectX::XMFLOAT3));

//   // Create the bounding box mesh, such that we can render it
//   DirectX::XMFLOAT3 corners[m_model_bounding_box.CORNER_COUNT];
//   m_model_bounding_box.GetCorners(corners);
//   std::vector<vertex_t> bounding_box_vertices;

//   // Create vertices from the corners of the bounding box
//   for (int i = 0; i < m_model_bounding_box.CORNER_COUNT; i++) {
//     bounding_box_vertices.push_back({ corners[i], { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } });
//   }

//   m_model_bounding_box_mesh = BoundingBoxMesh(bounding_box_vertices);
}

SceneNode::~SceneNode() {
 m_children.clear();
}

// void SceneNode::addChildNode(SceneNode &child) {
//   m_children.push_back(&child);
//   child.m_parent = this;
// }

void SceneNode::addChildNode(SceneNode *child) {
  m_children.push_back(child);
  child->m_parent = this;
}

void SceneNode::render(RenderContext& ctx) {
  if (!m_is_active) {
    return;
  }

  if (m_model) {
//     // Update the shader with the transform
//     m_model->m_shader.setModelMatrix(m_world_transform);
//     m_model->m_shader.setNormalRotationMatrix(m_world_rotation_matrix);

    // Keep track if there is an interaction with the model
    m_model->setInteractedState(m_intersected_in_current_frame);

    // Update the render context with the transform
    m_model->setWorldMatrix(m_world_transform);

    // And render the model
    m_model->render(ctx);
  }
  else if (m_renderable) {
    m_renderable->render(ctx);
  }

  for (SceneNode *child : m_children) {
    child->render(ctx);
  }
}

void SceneNode::updateTransformation() {
  // Update the `m_transform_needs_update` flag with the flag from parent node, as this node itself
  // might not have been changed, but its parent might be (which means this node needs to
  // update its transformation as well, which needs to be reflected in its children).
  m_transform_needs_update = m_transform_needs_update || m_parent && m_parent->m_transform_needs_update;

  // We only need to update the transform for the current element
  // if its `m_transform_needs_update` flag or the one of its parent is set to `true`
  if (m_transform_needs_update) {
    // Update the local transform
    m_local_transform = Geometry::composeWorldMatrix(
      m_translation,
      m_rotation,
      m_scaling
    );

    if (m_parent) {
      m_world_transform = m_parent->m_world_transform * m_local_transform;
      m_world_rotation_matrix = glm::toMat4(m_parent->m_rotation * m_rotation);
    }
    else {
      // No parent, is the root node, which means its local transform
      // is also its world transform, since the local transform is applied
      // relative to the origin point.
      m_world_transform = m_local_transform;
      m_world_rotation_matrix = glm::toMat4(m_rotation);
    }
  }

  // Update the transforms of all the children.
  for (SceneNode *child : m_children) {
    child->updateTransformation();
  }

  // And then reset the `m_transform_needs_update` flag
  m_transform_needs_update = false;
}

// DirectX::BoundingOrientedBox SceneNode::getTransformedBoundingBox() {
//   DirectX::BoundingOrientedBox transformed;
//   m_model_bounding_box.Transform(transformed, DirectX::XMMatrixTranspose(m_world_transform));
//   return transformed;
// }

void SceneNode::rotate(float roll, float pitch, float yaw) {
  auto rotation = glm::quat(glm::vec3(pitch, yaw, roll));
  rotate(rotation);
}

void SceneNode::rotate(glm::quat rotation) {
  m_rotation = m_rotation * rotation;
  m_transform_needs_update = true;
}

void SceneNode::translate(float x, float y, float z) {
  auto translation = glm::vec3({x, y, z});
  translate(translation);
}

void SceneNode::translate(glm::vec3 translation) {
  m_translation =m_translation + translation;
  m_transform_needs_update = true;
}

void SceneNode::scale(float x, float y, float z) {
  auto scaling = glm::vec3({x, y, z});
  scale(scaling);
}

void SceneNode::scale(glm::vec3 scaling) {
  m_scaling = m_scaling * scaling;
  m_transform_needs_update = true;
}

void SceneNode::setRotation(glm::quat rotation) {
  m_rotation = rotation;
  m_transform_needs_update = true;
}

void SceneNode::setScale(float x, float y, float z) {
  auto scaling = glm::vec3({x, y, z});
  setScale(scaling);
}

void SceneNode::setScale(glm::vec3 scaling) {
  m_scaling = scaling;
  m_transform_needs_update = true;
}

void SceneNode::setPosition(float x, float y, float z) {
 auto position = glm::vec3({x, y, z});
  setPosition(position);
}

void SceneNode::setPosition(glm::vec3 position) {
  m_translation = position;
  m_transform_needs_update = true;
}

glm::quat SceneNode::getRotation() {
  return m_rotation;
}

glm::vec3 SceneNode::getScale() {
  return m_scaling;
}

glm::vec3 SceneNode::getPosition() {
  return m_translation;
}

void SceneNode::setGrabbable(bool grabbable) {
  if (grabbable) {
    SceneNode::s_grabbable_instances.insert(this);
  }
  else {
    SceneNode::s_grabbable_instances.erase(this);
  }
}

std::unordered_set<SceneNode*> SceneNode::getGrabbableInstances() {
  std::unordered_set<SceneNode*> result;

  for (SceneNode *current_node : s_grabbable_instances) {
    if (current_node->isActive()) {
      result.insert(current_node);
    }
  }

  return result;
}

void SceneNode::setIsTerrain(bool is_terrain) {
  if (is_terrain) {
    SceneNode::s_terrain_instances.insert(this);
  }
  else {
    SceneNode::s_terrain_instances.erase(this);
  }
}

void SceneNode::setActive(bool is_active) {
  m_is_active = is_active;
}

bool SceneNode::isActive() {
  return m_is_active;
}

std::unordered_set<SceneNode*> SceneNode::getTerrainInstances() {
  std::unordered_set<SceneNode*> result;

  for (SceneNode *current_node : s_terrain_instances) {
    if (current_node->isActive()) {
      result.insert(current_node);
    }
  }

  return result;
}

bool SceneNode::intersects(SceneNode other) {
  return m_model->intersects(*other.m_model);
}

// bool SceneNode::intersects(DirectX::XMVECTOR line_start, DirectX::XMVECTOR line_direction, float *out_distance) {
//   return getTransformedBoundingBox().Intersects(line_start, line_direction, *out_distance);
// }

void SceneNode::resetInteractionStates() {
  for (SceneNode* current_node : s_grabbable_instances) {
    current_node->m_intersected_in_current_frame = false;
    current_node->m_grabbed = false;
  }
}
