#include "other_scene.h"
#include <xre/scene_manager.h>

void OtherScene::onActivate() {
  // --------------------------------------------------------------------
  // Create resources
  // --------------------------------------------------------------------
  texture = m_resource_manager->texture(DATA_FOLDER "textures/Wood048_2K-JPG_Color.jpg");

  basic_material = m_resource_manager->material(SHADERS_FOLDER "basic.vert.spv", SHADERS_FOLDER "basic.frag.spv");
  texture_material = m_resource_manager->material(SHADERS_FOLDER "basic.vert.spv", SHADERS_FOLDER "texture.frag.spv", texture);

  floor = m_resource_manager->plane(10, texture_material);

  root_node = std::make_shared<SceneNode>();
  floor_node = std::make_shared<SceneNode>(floor);

  button = m_resource_manager->button(
    basic_material,
    true,
    [this]() {
      std::cout << "Switching to main" << std::endl;
      m_scene_manager->setActive("main");
    }
  );

  // --------------------------------------------------------------------
  // Setup transformations etc.
  // --------------------------------------------------------------------

  // Setup scene graph
  root_node->addChildNode(floor_node);
  root_node->addChildNode(button->getRootNode());

  // Update initial transforms
  button->getRootNode()->setPosition({-1.0f, 1.3f, 0.0f});

  floor_node->setIsTerrain(true);

  root_node->updateTransformation();
}

void OtherScene::draw(RenderContext &ctx) {
  root_node->render(ctx);
}

void OtherScene::updateSimulation(XrTime predicted_time) {
  // Nothing to do
}