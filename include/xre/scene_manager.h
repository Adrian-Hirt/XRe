#pragma once

// XRe includes
#include <xre/structs.h>
#include <xre/utils.h>
#include <xre/scene.h>

// Other includes
#include <string>
#include <functional>
#include <memory>

class SceneManager {
public:
  using SceneFactory = std::function<std::unique_ptr<Scene>()>;

  void registerScene(std::string name, SceneFactory factory);
  void setActive(const std::string& name);
  void updateSimulation(XrTime predicted_time);
  void draw(RenderContext& ctx);
private:
  std::unordered_map<std::string, SceneFactory> m_scene_factories;
  std::unique_ptr<Scene> m_active_scene = nullptr;
};
