#pragma once

// XRe includes
#include <xre/mesh.h>
#include <xre/model.h>
#include <xre/text.h>
#include <xre/scene_node.h>
#include <xre/material.h>
#include <xre/texture.h>
#include <xre/components/grabbable_component.h>
#include <xre/components/button_component.h>
#include <xre/components/collider_component.h>
#include <xre/components/teleport_target_component.h>

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

  void registerComponent(Component &component);
  void unregisterComponent(Component &component);

  const std::vector<GrabbableComponent *> &getGrabbableComponents() const;
  const std::vector<ButtonComponent *> &getButtonComponents() const;
  const std::vector<TeleportTargetComponent *> &getTeleportTargetComponents() const;

protected:
  // Keep track of resource manager to create resources such as models or materials
  std::shared_ptr<ResourceManager> m_resource_manager;

  // Set of all grabbable components in this scene
  std::vector<GrabbableComponent *> m_grabbable_components;

  // Set of all scene nodes belonging to this scene we marked as terrain (i.e. can teleport there)
  std::vector<TeleportTargetComponent *> m_teleport_target_components;

  // Set of all button components in the scene
  std::vector<ButtonComponent *> m_button_components;
};
