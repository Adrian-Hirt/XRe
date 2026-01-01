#pragma once

// XRe includes
#include <xre/mesh.h>
#include <xre/model.h>
#include <xre/text.h>
#include <xre/scene_node.h>
#include <xre/material.h>
#include <xre/texture.h>
#include <xre/resource_manager.h>
#include <xre/button.h>

// Other includes
#include <memory>

// Forward declaration
class SceneManager;

class Scene {
public:
  Scene(std::shared_ptr<ResourceManager> resource_manager, std::shared_ptr<SceneManager> scene_manager) : m_resource_manager(resource_manager), m_scene_manager(scene_manager) {};

  virtual void onActivate() {};
  virtual void onDeactivate() {};

  // Override this method to draw some stuff
  virtual void draw(RenderContext &ctx) {};

  // Override this method to update the simulation based
  // on the predicted time the frame will be rendered
  virtual void updateSimulation(XrTime predicted_time) {};

protected:
  std::shared_ptr<ResourceManager> m_resource_manager;

  std::shared_ptr<SceneManager> m_scene_manager;
};
