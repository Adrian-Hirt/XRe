#pragma once

// DirectX includes
#include <d3d11.h>
#include <DirectXMath/DirectXMath.h>

// XRe includes
#include <xre/model.h>
#include <xre/shader.h>

// Other includes
#include <vector>

class SceneNode {
public:
  SceneNode();
  SceneNode(Model* model);
  SceneNode(Model* model, SceneNode &parent);
  ~SceneNode();

  void addChildNode(SceneNode &child);
  void addChildNode(SceneNode *child);
  void render();
  void updateTransformation();
  void buildBoundingBox();
  DirectX::BoundingOrientedBox getTransformedBoundingBox();

  // These methods apply the rotation / translation / scaling
  // to the values we already have, e.g. to simply double the
  // scale you could pass in a value of 2 to the `scale` method
  void rotate(float roll, float pitch, float yaw);
  void rotate(DirectX::XMVECTOR rotation);
  void translate(float x, float y, float z);
  void translate(DirectX::XMVECTOR translation);
  void scale(float x, float y, float z);
  void scale(DirectX::XMVECTOR scaling);

  // These methods set the rotation / translation / scaling to
  // the values passed in, use this if you already calculated the
  // new values to use beforehand.
  void setRotation(DirectX::XMVECTOR rotation);
  void setScale(float x, float y, float z);
  void setScale(DirectX::XMVECTOR scaling);
  void setPosition(float x, float y, float z);
  void setPosition(DirectX::XMVECTOR position);

  // Getters for position, scale and rotation
  DirectX::XMVECTOR getRotation();
  DirectX::XMVECTOR getScale();
  DirectX::XMVECTOR getPosition();

  static std::unordered_set<SceneNode*> getGrabbableInstances();
  void setGrabbable(bool grabbable);
  static std::unordered_set<SceneNode*> getTerrainInstances();
  void setIsTerrain(bool is_terrain);

  void setActive(bool is_active);
  bool isActive();

  // Check whether a node intersects with a bounding box
  bool intersects(DirectX::BoundingOrientedBox other);

  // Check whether a node intersects with a line (and optionally put the
  // distance of the intersection point as an out parameter)
  bool intersects(DirectX::XMVECTOR line_start, DirectX::XMVECTOR line_direction, float *out_distance);

  bool m_grabbed = false;
  bool m_intersected_in_current_frame = false;

  static void resetInteractionStates();
private:
  // Parent node (which might be null for the root node)
  SceneNode* m_parent;

  // Children of this node
  std::vector<SceneNode *> m_children;

  // Model contained in this node (which might be null, e.g. for a node only containing some
  // transformation)
  Model* m_model;

  // Position, scale and rotation of the SceneNode. These are all LOCAL,
  // i.e. in relation to the transform of the parent node!
  // Scale factors, initialize to use the scale of 1 for X, Y and Z
  DirectX::XMVECTOR m_scaling = DirectX::XMVECTORF32({1.0f, 1.0f, 1.0f});
  // The position of the object in world coordinates
  DirectX::XMVECTOR m_translation = DirectX::XMVECTORF32({0.0f, 0.0f, 0.0f});
  // The rotation of the SceneNode
  DirectX::XMVECTOR m_rotation = DirectX::XMQuaternionIdentity();

  // Transform relative to world coordinates
  DirectX::XMMATRIX m_world_transform;

  // Transform relative to parent
  DirectX::XMMATRIX m_local_transform;

  // Rotation relative to world
  DirectX::XMMATRIX m_world_rotation_matrix;

  DirectX::BoundingOrientedBox m_model_bounding_box;
  BoundingBoxMesh m_model_bounding_box_mesh;

  // Set of all instances we marked as grabbable
  inline static std::unordered_set<SceneNode*> s_grabbable_instances;

  inline static std::unordered_set<SceneNode*> s_terrain_instances;

  bool m_transform_needs_update = true;

  bool m_is_active = true;
};