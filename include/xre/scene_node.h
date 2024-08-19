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
  ~SceneNode();

  void addChildNode(SceneNode &child);
  void render();
  void updateTransformation();

private:
  // Parent node (which might be null for the root node)
  SceneNode* m_parent;

  // Children of this node
  std::vector<SceneNode *> m_children;

  // Model contained in this node (which might be null, e.g. for a node only containing some
  // transformation)
  Model* m_model;

  // TODO: remove when models have a default shader
  Shader m_shader;

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
};