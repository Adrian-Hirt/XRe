#pragma once

// XRe includes
#include <xre/model.h>
#include <xre/renderable.h>

// Other includes
#include <vector>
#include <glm/glm/vec3.hpp>
#include <unordered_set>

class SceneNode {
public:
  SceneNode();
  SceneNode(Model *model);
  SceneNode(Model *model, SceneNode &parent);
  SceneNode(Renderable *model);
  SceneNode(Renderable *model, SceneNode &parent);
  ~SceneNode();

  void addChildNode(SceneNode *child);
  void render(RenderContext &ctx);
  void updateTransformation();

  // These methods apply the rotation / translation / scaling
  // to the values we already have, e.g. to simply double the
  // scale you could pass in a value of 2 to the `scale` method
  void rotate(float roll, float pitch, float yaw);
  void rotate(glm::quat rotation);
  void translate(float x, float y, float z);
  void translate(glm::vec3 translation);
  void scale(float x, float y, float z);
  void scale(glm::vec3 scaling);

  // These methods set the rotation / translation / scaling to
  // the values passed in, use this if you already calculated the
  // new values to use beforehand.
  void setRotation(glm::quat rotation);
  void setScale(float x, float y, float z);
  void setScale(glm::vec3 scaling);
  void setPosition(float x, float y, float z);
  void setPosition(glm::vec3 position);

  // Getters for position, scale and rotation
  glm::quat getRotation();
  glm::vec3 getScale();
  glm::vec3 getPosition();

  static std::unordered_set<SceneNode *> getGrabbableInstances();
  void setGrabbable(bool grabbable);
  static std::unordered_set<SceneNode *> getTerrainInstances();
  void setIsTerrain(bool is_terrain);

  void setActive(bool is_active);
  bool isActive();

  // Check whether a node intersects with the model contained in another one
  bool intersects(SceneNode other);

  // Check whether a node intersects with a line (and optionally put the
  // distance of the intersection point as an out parameter)
  bool intersects(const glm::vec3 &line_start, const glm::vec3 &line_direction, float *out_distance);

  bool m_grabbed = false;
  bool m_intersected_in_current_frame = false;

  static void resetInteractionStates();

private:
  // Parent node (which might be null for the root node)
  SceneNode *m_parent;

  // Children of this node
  std::vector<SceneNode *> m_children;

  // Model or Renderable contained in this node (which might be null, e.g. for a node only containing some
  // transformation)
  Model *m_model = nullptr;
  Renderable *m_renderable = nullptr;

  // Position, scale and rotation of the SceneNode. These are all LOCAL,
  // i.e. in relation to the transform of the parent node!
  // Scale factors, initialize to use the scale of 1 for X, Y and Z
  glm::vec3 m_scaling = glm::one<glm::vec3>();
  // The position of the object in world coordinates
  glm::vec3 m_translation = glm::zero<glm::vec3>();
  // The rotation of the SceneNode
  glm::quat m_rotation = glm::identity<glm::quat>();

  // Transform relative to world coordinates
  glm::mat4 m_world_transform;

  // Transform relative to parent
  glm::mat4 m_local_transform;

  // Rotation relative to world
  glm::mat4 m_world_rotation_matrix;

  // Set of all instances we marked as grabbable
  inline static std::unordered_set<SceneNode *> s_grabbable_instances;

  inline static std::unordered_set<SceneNode *> s_terrain_instances;

  bool m_transform_needs_update = true;

  bool m_is_active = true;
};