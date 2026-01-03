#pragma once

// XRe includes
#include <xre/openxr_handler.h>
#include <xre/renderable.h>
#include <xre/mesh.h>
#include <xre/model.h>
#include <xre/text.h>
#include <xre/scene_node.h>
#include <xre/material.h>
#include <xre/texture.h>
#include <xre/resource_manager.h>
#include <xre/button.h>
#include <xre/scene.h>
#include <xre/scene_manager.h>

// Other includes
#include <memory>
#include <functional>

class Application {
public:
  Application(const char *application_name);
  ~Application();

  void run();
  virtual void setup();
  virtual void draw(RenderContext &ctx);
  virtual void updateSimulation(XrTime predicted_time);

  std::shared_ptr<ResourceManager> resourceManager();

private:
  // Handlers
  std::unique_ptr<OpenXrHandler> m_open_xr_handler;

  // Resource manager
  std::shared_ptr<ResourceManager> m_resource_manager;
};
