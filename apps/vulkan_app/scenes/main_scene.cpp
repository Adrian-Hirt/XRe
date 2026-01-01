#include "main_scene.h"

void MainScene::onActivate() {
  std::cout << "Setup" << std::endl;

  // --------------------------------------------------------------------
  // Create resources
  // --------------------------------------------------------------------
  texture = m_resource_manager->texture(DATA_FOLDER "textures/Wood048_2K-JPG_Color.jpg");
  texture2 = m_resource_manager->texture(DATA_FOLDER "textures/Tiles012_2K-JPG_Color.jpg");
  texture3 = m_resource_manager->texture(DATA_FOLDER "textures/Bricks090_2K-JPG_Color.jpg");

  material = m_resource_manager->material(SHADERS_FOLDER "ambient.vert.spv", SHADERS_FOLDER "basic.frag.spv");
  basic_material = m_resource_manager->material(SHADERS_FOLDER "basic.vert.spv", SHADERS_FOLDER "basic.frag.spv");
  texture_material = m_resource_manager->material(SHADERS_FOLDER "basic.vert.spv", SHADERS_FOLDER "texture.frag.spv", texture);
  texture2_material = m_resource_manager->material(SHADERS_FOLDER "basic.vert.spv", SHADERS_FOLDER "texture.frag.spv", texture2);
  static_image_material = m_resource_manager->material(SHADERS_FOLDER "bitmap.vert.spv", SHADERS_FOLDER "texture.frag.spv", texture3);

  cube1 = m_resource_manager->cube(basic_material, {0.0f, 1.0f, 0.0f});
  cube2 = m_resource_manager->cube(material, {0.0f, 1.0f, 0.0f});
  sphere1 = m_resource_manager->sphere(material);
  sphere2 = m_resource_manager->sphere(material);
  quad = m_resource_manager->quad(static_image_material);
  floor = m_resource_manager->plane(10, texture_material);
  cube = m_resource_manager->cube(texture2_material);
  line = m_resource_manager->line(0.003f, 2.0f, material, {1.0f, 0.0f, 0.0f});

  // Create some text with some valid and some invalid characters
  text = m_resource_manager->text("This is a sample text :) カタカナ", true);

  root_node = std::make_shared<SceneNode>();
  floor_node = std::make_shared<SceneNode>(floor);
  cube1_node = std::make_shared<SceneNode>(cube1);
  cube2_node = std::make_shared<SceneNode>(cube2);
  sphere1_node = std::make_shared<SceneNode>(sphere1);
  sphere2_node = std::make_shared<SceneNode>(sphere2);
  cube_node = std::make_shared<SceneNode>(cube);
  text_node = std::make_shared<SceneNode>(text->getModel());
  quad_node = std::make_shared<SceneNode>(quad);

  button = m_resource_manager->button(
    basic_material,
    [this]() {
      std::cout << "Press" << std::endl;
    }
  );

  // --------------------------------------------------------------------
  // Setup transformations etc.
  // --------------------------------------------------------------------
  sphere1->setColor({1.0f, 0.0f, 0.0f});
  sphere2->setColor({1.0f, 0.0f, 0.0f});
  floor->setColor({0.7f, 0.7f, 0.7f});

  // Setup scene graph
  root_node->addChildNode(cube1_node);
  root_node->addChildNode(cube2_node);
  root_node->addChildNode(floor_node);
  root_node->addChildNode(sphere1_node);
  root_node->addChildNode(sphere2_node);
  root_node->addChildNode(cube_node);
  root_node->addChildNode(text_node);
  root_node->addChildNode(quad_node);
  root_node->addChildNode(button->getRootNode());

  // Update initial transforms
  cube1_node->setScale(0.1f, 0.1f, 0.1f);
  cube2_node->setScale(0.1f, 0.1f, 0.1f);
  cube1_node->setPosition(0.0f, 1.5f, -2.0f);
  cube2_node->setPosition(0.0f, 1.5f, 0.0f);
  sphere1_node->scale(0.1f, 0.1f, 0.1f);
  sphere2_node->scale(0.1f, 0.1f, 0.1f);
  sphere2_node->setPosition(5.0f, 0.0f, -4.0f);
  cube_node->scale(2.0f, 0.5f, 2.0f);
  cube_node->setPosition(4.0f, 0.5f, 4.0f);
  button->getRootNode()->setPosition({1.0f, 1.3f, 0.0f});

  text_node->scale(0.2f, 0.2f, 0.2f);
  text_node->translate(0.0f, 0.2f, 0.0f);

  quad_node->translate(0.8f, 0.8f, 0.0f);
  quad_node->scale(0.1f, 0.1f, 0.1f);

  cube1_node->setGrabbable(true);
  floor_node->setIsTerrain(true);
  cube_node->setIsTerrain(true);

  root_node->updateTransformation();
}

void MainScene::draw(RenderContext &ctx) {
  root_node->render(ctx);

  if (cube1_node->intersects(cube2_node)) {
    cube1->setColor({1.0f, 0.0f, 0.0f});
  } else {
    cube1->resetColor();
  }

  line->render(ctx);
}

void MainScene::updateSimulation(XrTime predicted_time) {
  // compute delta time in seconds
  float dt = 0.0f;

  // skip first frame
  if (last_time != 0) {
    dt = static_cast<float>(predicted_time - last_time) * 1e-9f; // assuming XrTime is in ns
  }
  last_time = predicted_time;

  // movement speed in units per second
  constexpr float rotation_speed = 1.0f;
  float translation = rotation_speed * dt;

  if (forward) {
    cube2_node->translate(0.0f, 0.0f, -translation);
  } else {
    cube2_node->translate(0.0f, 0.0f, translation);
  }

  float z = cube2_node->getPosition()[2];

  if (forward) {
    if (z < -3.0f) {
      forward = false;
    }
  }
  else {
    if (z > 0) {
      forward = true;
    }
  }

  // Update transformation
  root_node->updateTransformation();
}