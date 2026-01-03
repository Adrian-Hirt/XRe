#pragma once

// XRe includes
#include <xre/structs.h>
#include <xre/utils.h>
#include <xre/scene.h>
#include <xre/vulkan_handler.h>

// Other includes
#include <string>
#include <functional>
#include <memory>

class SceneManager {
public:
  // Convenience alias
  using SceneFactory = std::function<std::unique_ptr<Scene>()>;

  // Delete copy/move and default constructor
  SceneManager(const SceneManager&) = delete;
  SceneManager& operator=(const SceneManager&) = delete;
  SceneManager() = delete;

  // Singleton methods
  static void init(std::shared_ptr<VulkanHandler> vulkan_handler);
  static SceneManager& instance();

  // Method to register a new scene
  void registerScene(std::string name, SceneFactory factory);

  // Set a previously registered scene as the active scene
  void setActive(const std::string& name);

  // Will forward the calls to the active scene
  void updateSimulation(XrTime predicted_time);
  void draw(RenderContext& ctx);
  void resetInteractionStates();
  std::unordered_set<SceneNode *> getGrabbableNodeInstances();
  std::unordered_set<SceneNode *> getTerrainInstances();
private:
  std::unordered_map<std::string, SceneFactory> m_scene_factories;
  std::unique_ptr<Scene> m_active_scene = nullptr;
  std::shared_ptr<VulkanHandler> m_vulkan_handler;

  // Constructor is private
  SceneManager(std::shared_ptr<VulkanHandler> vulkan_handler);

  // Instance for singleton pattern
  inline static std::unique_ptr<SceneManager> s_instance = nullptr;
};
