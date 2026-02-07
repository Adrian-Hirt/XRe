#pragma once

// XRe includes
#include <xre/mesh.h>
#include <xre/model.h>
#include <xre/text.h>
#include <xre/scene_node.h>
#include <xre/material.h>
#include <xre/texture.h>
#include <xre/grabbable_component.h>
#include <xre/button_component.h>

// Other includes
#include <memory>

// Forward declarations
class SceneManager;
class Button;
class ResourceManager;

class Scene {
public:
  Scene(std::shared_ptr<ResourceManager> resource_manager) : m_resource_manager(resource_manager) {};

  virtual void onActivate() {};
  virtual void onDeactivate() {};

  // Override this method to draw some stuff
  virtual void draw(RenderContext &ctx) {};

  // Override this method to update the simulation based
  // on the predicted time the frame will be rendered
  virtual void updateSimulation(XrTime predicted_time) {};

  // Convenience methods that automatically set the scene of the node
  std::shared_ptr<SceneNode> node();
  std::shared_ptr<SceneNode> node(std::shared_ptr<Model> model);

  void setNodeIsTerrain(SceneNode *node, bool is_terrain);
  std::unordered_set<SceneNode *> getTerrainNodeInstances();

  std::unordered_set<Button *> getButtonInstances();
  void processButtonInteractions();
  void resetButtonInteractions();

  void addButton(Button *button);

  void registerComponent(Component &component);
  void unregisterComponent(Component &component);

  const std::vector<GrabbableComponent *> &getGrabbableComponents() const;
  const std::vector<ButtonComponent *> &getButtonComponents() const;

protected:
  // Keep track of resource manager to create resources such as models or materials
  std::shared_ptr<ResourceManager> m_resource_manager;

  // Set of all grabbable components in this scene
  std::vector<GrabbableComponent *> m_grabbable_components;

  // Set of all scene nodes belonging to this scene we marked as terrain (i.e. can teleport there)
  std::unordered_set<SceneNode *> m_terrain_scene_nodes;

  // Set of all buttons in the scene
  std::unordered_set<Button *> m_button_instances;
  std::vector<ButtonComponent *> m_button_components;
};
