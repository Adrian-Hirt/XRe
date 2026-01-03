#pragma once

// XRe includes
#include <xre/model.h>
#include <xre/structs.h>
#include <xre/model.h>
#include <xre/vulkan_handler.h>
#include <xre/scene_node.h>

// Other includes
#include <memory>
#include <string>

class Text {
public:
  Text(const std::string sentence, std::shared_ptr<VulkanHandler> vulkan_handler, bool stick_to_hud);
  std::shared_ptr<SceneNode> getSceneNode();

private:
  // Using the extendes ASCII charset for now, which has 224 characters (skipping control character)
  const int CHAR_COUNT = 224;

  // Steps for going through the texture
  const float X_STEP = 1.0f / 32.0f;
  const float Y_STEP = 1.0f / 7.0f;

  // Keep track whether the text should be sticking to the HUD or if it
  // should be displayed in the world
  bool m_stick_to_hud = false;

  void buildMeshesFromSentence(const std::string sentence);
  inline TextChar computeTextureOffsets(int letter);

  std::shared_ptr<VulkanHandler> m_vulkan_handler;
  std::shared_ptr<Model> m_model;

  // Scene node for the text
  std::shared_ptr<SceneNode> m_scene_node;
};
