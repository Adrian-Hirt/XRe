#include "other_scene.h"

void OtherScene::onActivate() {
  // --------------------------------------------------------------------
  // Create resources
  // --------------------------------------------------------------------
  texture = m_resource_manager->texture(DATA_FOLDER "textures/Wood048_2K-JPG_Color.jpg");

  basic_material = m_resource_manager->material(SHADERS_FOLDER "basic.vert.spv", SHADERS_FOLDER "basic.frag.spv");
  texture_material = m_resource_manager->material(SHADERS_FOLDER "basic.vert.spv", SHADERS_FOLDER "texture.frag.spv", texture);

  floor = m_resource_manager->plane(10, texture_material);
  button = m_resource_manager->cube(basic_material, {1.0f, 0.0f, 0.0f});

  root_node = node();
  floor_node = node(floor);
  button_node = node(button);

  button_node->addComponent(
    std::make_unique<ButtonComponent>(
      [this]() {
        std::cout << "Switching to main" << std::endl;
      SceneManager::instance().setActive("main");
      }
    )
  );

  // --------------------------------------------------------------------
  // Setup transformations etc.
  // --------------------------------------------------------------------

  // Setup scene graph
  root_node->addChildNode(floor_node);
  root_node->addChildNode(button_node);

  // Update initial transforms
  button_node->setPosition({-1.0f, 1.3f, 0.0f});
  button_node->scale(0.1f, 0.1f, 0.02f);

  floor_node->addComponent(std::make_unique<TeleportTargetComponent>());
  floor_node->addComponent(ColliderComponent::fromPointGroups(floor->getVectorPositionsPerMesh()));

  root_node->updateTransformation();
}

void OtherScene::draw(RenderContext &ctx) {
  root_node->render(ctx);
}

void OtherScene::updateSimulation(XrTime predicted_time) {
  // Nothing to do
}