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

void SceneNode::buildBoundingBox() {
  std::vector<DirectX::XMFLOAT3> bounding_box_corners = m_model->getMeshBoundingBoxCorners();
  size_t points_count = bounding_box_corners.size();

  // Build the bounding box
  DirectX::BoundingOrientedBox::CreateFromPoints(m_model_bounding_box, points_count, bounding_box_corners.data(), sizeof(DirectX::XMFLOAT3));

  // Create the bounding box mesh, such that we can render it
  DirectX::XMFLOAT3 corners[m_model_bounding_box.CORNER_COUNT];
  m_model_bounding_box.GetCorners(corners);
  std::vector<vertex_t> bounding_box_vertices;

  // Create vertices from the corners of the bounding box
  for (int i = 0; i < m_model_bounding_box.CORNER_COUNT; i++) {
    bounding_box_vertices.push_back({ corners[i], { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } });
  }

  m_model_bounding_box_mesh = BoundingBoxMesh(bounding_box_vertices);
}

SceneNode::~SceneNode() {
 m_children.clear();
}

void SceneNode::addChildNode(SceneNode &child) {
  m_children.push_back(&child);
  child.m_parent = this;
}

void SceneNode::addChildNode(SceneNode *child) {
  m_children.push_back(child);
  child->m_parent = this;
}

void SceneNode::render() {
  if (!m_render) {
    return;
  }

  if (m_model) {
    m_model->m_shader.activate();

    // Update the shader with the transform
    m_model->m_shader.setModelMatrix(m_world_transform);
    m_model->m_shader.setNormalRotationMatrix(m_world_rotation_matrix);
    if (m_intersected_in_current_frame) {
      m_model->m_shader.setModelColor({1.0f, 0.0f, 0.0f, 1.0f});
    }
    else {
      m_model->m_shader.setModelColor(m_model->getColor());
    }
    m_model->m_shader.updatePerModelConstantBuffer();
    m_model->render();

    // Set shader variables to identities, as the bounding box is already updated
    // with the correct position (as we need the correct position to be able to
    // compute intersections).
    m_model_bounding_box_mesh.render();
  }

  for (SceneNode *child : m_children) {
    child->render();
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

    if (m_model) {
      m_model_bounding_box_mesh.updateVerticesFromBoundingBox(getTransformedBoundingBox());
    }
  }

  // Update the transforms of all the children.
  for (SceneNode *child : m_children) {
    child->updateTransformation();
  }

  // And then reset the `m_transform_needs_update` flag
  m_transform_needs_update = false;
}

DirectX::BoundingOrientedBox SceneNode::getTransformedBoundingBox() {
  DirectX::BoundingOrientedBox transformed;
  m_model_bounding_box.Transform(transformed, DirectX::XMMatrixTranspose(m_world_transform));
  return transformed;
}

void SceneNode::rotate(float roll, float pitch, float yaw) {
  DirectX::XMVECTOR rotation = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
  rotate(rotation);
}

void SceneNode::rotate(DirectX::XMVECTOR rotation) {
  m_rotation = DirectX::XMQuaternionMultiply(m_rotation, rotation);
  m_transform_needs_update = true;
}

void SceneNode::translate(float x, float y, float z) {
  DirectX::XMVECTOR translation = DirectX::XMVECTORF32({x, y, z});
  translate(translation);
}

void SceneNode::translate(DirectX::XMVECTOR translation) {
  m_translation = DirectX::XMVectorAdd(m_translation, translation);
  m_transform_needs_update = true;
}

void SceneNode::scale(float x, float y, float z) {
  DirectX::XMVECTOR scaling = DirectX::XMVECTORF32({x, y, z});
  scale(scaling);
}

void SceneNode::scale(DirectX::XMVECTOR scaling) {
  m_scaling = DirectX::XMVectorMultiply(m_scaling, scaling);
  m_transform_needs_update = true;
}

void SceneNode::setRotation(DirectX::XMVECTOR rotation) {
  m_rotation = rotation;
  m_transform_needs_update = true;
}

void SceneNode::setScale(float x, float y, float z) {
  DirectX::XMVECTOR scaling = DirectX::XMVECTORF32({x, y, z});
  setScale(scaling);
}

void SceneNode::setScale(DirectX::XMVECTOR scaling) {
  m_scaling = scaling;
  m_transform_needs_update = true;
}

void SceneNode::setPosition(float x, float y, float z) {
  DirectX::XMVECTOR position = DirectX::XMVECTORF32({x, y, z});
  setPosition(position);
}

void SceneNode::setPosition(DirectX::XMVECTOR position) {
  m_translation = position;
  m_transform_needs_update = true;
}

DirectX::XMVECTOR SceneNode::getRotation() {
  return m_rotation;
}

DirectX::XMVECTOR SceneNode::getScale() {
  return m_scaling;
}

DirectX::XMVECTOR SceneNode::getPosition() {
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
  return s_grabbable_instances;
}

void SceneNode::setIsTerrain(bool is_terrain) {
  if (is_terrain) {
    SceneNode::s_terrain_instances.insert(this);
  }
  else {
    SceneNode::s_terrain_instances.erase(this);
  }
}

std::unordered_set<SceneNode*> SceneNode::getTerrainInstances() {
  return s_terrain_instances;
}

bool SceneNode::intersects(DirectX::BoundingOrientedBox other) {
  return getTransformedBoundingBox().Intersects(other);
}

bool SceneNode::intersects(DirectX::XMVECTOR line_start, DirectX::XMVECTOR line_direction, float *out_distance) {
  return getTransformedBoundingBox().Intersects(line_start, line_direction, *out_distance);
}

void SceneNode::resetInteractionStates() {
  for (SceneNode* current_node : s_grabbable_instances) {
    current_node->m_intersected_in_current_frame = false;
    current_node->m_grabbed = false;
  }
}
