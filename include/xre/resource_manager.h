#pragma once

// XRe includes
#include <xre/vulkan_handler.h>
#include <xre/texture.h>
#include <xre/material.h>
#include <xre/model_factory.h>
#include <xre/line.h>
#include <xre/text.h>

// Other includes
#include <memory>
#include <string>

class ResourceManager {
public:
  ResourceManager(std::shared_ptr<VulkanHandler> vulkan_handler);

  std::shared_ptr<Texture> texture(const std::string &path);

  // Methods to create materials
  std::shared_ptr<Material> material(const std::string &vert_path, const std::string &frag_path);
  std::shared_ptr<Material> material(const std::string &vert_path, const std::string &frag_path, std::shared_ptr<Texture> texture);

  // Methods to create models that we have predefined
  // TODO: return unique ptrs and handle ownership correctly
  std::shared_ptr<Model> cube(std::shared_ptr<Material> material);
  std::shared_ptr<Model> cube(std::shared_ptr<Material> material, glm::vec3 color);
  std::shared_ptr<Model> sphere(std::shared_ptr<Material> material);
  std::shared_ptr<Model> sphere(std::shared_ptr<Material> material, glm::vec3 color);
  std::shared_ptr<Model> plane(float extent, std::shared_ptr<Material> material);
  std::shared_ptr<Model> plane(float extent, std::shared_ptr<Material> material, glm::vec3 color);

  // Method to create a model from loading the geometry from a .obj file
  std::shared_ptr<Model> model(const char *model_path, std::shared_ptr<Material> material);
  std::shared_ptr<Model> model(const char *model_path, std::shared_ptr<Material> material, glm::vec3 color);

  // Methods to create lines
  std::shared_ptr<Line> line(float thickness, float length, std::shared_ptr<Material> material);
  std::shared_ptr<Line> line(float thickness, float length, std::shared_ptr<Material> material, glm::vec3 color);

  // Methods to create text
  std::shared_ptr<Text> text(std::string sentence);

private:
  std::shared_ptr<VulkanHandler> m_vulkan_handler;

  inline static const glm::vec3 DEFAULT_MODEL_COLOR = {0.8f, 0.8f, 0.8f};
};
