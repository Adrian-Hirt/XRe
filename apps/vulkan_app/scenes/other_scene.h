#pragma once

// Include scene base class
#include <xre/scene.h>

class OtherScene : public Scene {
public:
  OtherScene(std::shared_ptr<ResourceManager> resource_manager) : Scene(resource_manager) {};

  void onActivate();
  void draw(RenderContext &ctx);
  void updateSimulation(XrTime predicted_time);
private:
  std::shared_ptr<Texture> texture;
  std::shared_ptr<Material> basic_material;
  std::shared_ptr<Material> texture_material;

  std::shared_ptr<Model> floor;

  std::shared_ptr<SceneNode> root_node;
  std::shared_ptr<SceneNode> floor_node;

  std::shared_ptr<Button> button;
};
