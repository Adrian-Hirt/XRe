#pragma once

// XRe includes
#include <xre/model.h>
#include <xre/structs.h>
#include <xre/model.h>
#include <xre/vulkan_handler.h>

// Other includes
#include <memory>
#include <string>

class Text {
public:
  Text(const std::string sentence, std::shared_ptr<VulkanHandler> vulkan_handler);
  std::shared_ptr<Model> getModel();

private:
  // Using the extendes ASCII charset for now, which has 224 characters (skipping control character)
  const int CHAR_COUNT = 224;

  // Steps for going through the texture
  const float X_STEP = 1.0f / 32.0f;
  const float Y_STEP = 1.0f / 7.0f;

  void buildMeshesFromSentence(const std::string sentence);
  inline TextChar computeTextureOffsets(int letter);

  std::shared_ptr<VulkanHandler> m_vulkan_handler;
  std::shared_ptr<Model> m_model;
};
