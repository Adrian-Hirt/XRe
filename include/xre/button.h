#pragma once

// XRe includes
#include <xre/vulkan_handler.h>
#include <xre/model.h>
#include <xre/scene_node.h>
#include <xre/model_factory.h>
#include <xre/material.h>

// Other includes
#include <memory>
#include <functional>

class Button {
public:
  Button(std::shared_ptr<Material> material, bool disable_on_trigger, std::function<void()>trigger_callback, std::shared_ptr<VulkanHandler> vulkan_handler);
  std::shared_ptr<SceneNode> getRootNode();
  void trigger();
  static std::unordered_set<Button *> getInstances();
  static void processButtonTriggers();
  static void resetInteractionStates();

private:
  // Model of the button
  std::shared_ptr<Model> m_model;

  // Scene Node of the button
  std::shared_ptr<SceneNode> m_root_node;

  // Callback when button is triggered
  std::function<void()> m_trigger_callback;

  // All instances of the buttons
  inline static std::unordered_set<Button *> s_instances;

  // Track if button is enabled or not
  bool m_enabled = true;
  bool m_disable_on_trigger = false;
};
