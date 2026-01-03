#pragma once

// Include scene base class
#include <xre/scene.h>
#include <xre/scene_manager.h>
#include <xre/resource_manager.h>

class MainScene : public Scene {
public:
  MainScene(std::shared_ptr<ResourceManager> resource_manager) : Scene(resource_manager) {};

  void onActivate();
  void draw(RenderContext &ctx);
  void updateSimulation(XrTime predicted_time);
private:
  std::shared_ptr<Texture> texture;
  std::shared_ptr<Texture> texture2;
  std::shared_ptr<Texture> texture3;

  std::shared_ptr<Material> material;
  std::shared_ptr<Material> basic_material;
  std::shared_ptr<Material> texture_material;
  std::shared_ptr<Material> texture2_material;
  std::shared_ptr<Material> static_image_material;

  std::shared_ptr<Model> cube1;
  std::shared_ptr<Model> cube2;
  std::shared_ptr<Model> sphere1;
  std::shared_ptr<Model> sphere2;
  std::shared_ptr<Model> quad;
  std::shared_ptr<Model> floor;
  std::shared_ptr<Model> cube;
  std::shared_ptr<Line> line;

  std::shared_ptr<Text> text;

  std::shared_ptr<SceneNode> root_node;
  std::shared_ptr<SceneNode> floor_node;
  std::shared_ptr<SceneNode> cube1_node;
  std::shared_ptr<SceneNode> cube2_node;
  std::shared_ptr<SceneNode> sphere1_node;
  std::shared_ptr<SceneNode> sphere2_node;
  std::shared_ptr<SceneNode> cube_node;
  std::shared_ptr<SceneNode> text_node;
  std::shared_ptr<SceneNode> quad_node;

  std::shared_ptr<Button> button;

  XrTime last_time = 0;
  bool forward = true;
};
