#include <xre/application.h>

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};

  ResourceManager& res = resourceManager();

  std::shared_ptr<Texture> texture = res.texture(DATA_FOLDER "textures/Wood048_2K-JPG_Color.jpg");
  std::shared_ptr<Texture> texture2 = res.texture(DATA_FOLDER "textures/Tiles012_2K-JPG_Color.jpg");
  std::shared_ptr<Texture> texture3 = res.texture(DATA_FOLDER "textures/Bricks090_2K-JPG_Color.jpg");

  std::shared_ptr<Material> material = res.material(SHADERS_FOLDER "vk/ambient.vert.spv", SHADERS_FOLDER "vk/basic.frag.spv");
  std::shared_ptr<Material> basic_material = res.material(SHADERS_FOLDER "vk/basic.vert.spv", SHADERS_FOLDER "vk/basic.frag.spv");
  std::shared_ptr<Material> texture_material = res.material(SHADERS_FOLDER "vk/texture.vert.spv", SHADERS_FOLDER "vk/texture.frag.spv", texture);
  std::shared_ptr<Material> texture2_material = res.material(SHADERS_FOLDER "vk/texture.vert.spv", SHADERS_FOLDER "vk/texture.frag.spv", texture2);
  std::shared_ptr<Material> static_image_material = res.material(SHADERS_FOLDER "vk/bitmap.vert.spv", SHADERS_FOLDER "vk/texture.frag.spv", texture3);

  std::shared_ptr<Model> cube1 = res.cube(basic_material, {0.0f, 1.0f, 0.0f});
  std::shared_ptr<Model> cube2 = res.cube(material, {0.0f, 1.0f, 0.0f});
  std::shared_ptr<Model> sphere1 = res.sphere(material);
  std::shared_ptr<Model> sphere2 = res.sphere(material);
  std::shared_ptr<Model> quad = res.quad(static_image_material);
  std::shared_ptr<Model> floor = res.plane(10, texture_material);
  std::shared_ptr<Model> cube = res.cube(texture2_material);
  std::shared_ptr<Line> line = res.line(0.003f, 2.0f, material, {1.0f, 0.0f, 0.0f});
  XrTime last_time = 0;

  // Create some text with some valid and some invalid characters
  std::shared_ptr<Text> text = res.text("This is a sample text :) カタカナ", true);

  std::shared_ptr<SceneNode> root_node = std::make_shared<SceneNode>();
  std::shared_ptr<SceneNode> floor_node = std::make_shared<SceneNode>(floor);
  std::shared_ptr<SceneNode> cube1_node = std::make_shared<SceneNode>(cube1);
  std::shared_ptr<SceneNode> cube2_node = std::make_shared<SceneNode>(cube2);
  std::shared_ptr<SceneNode> sphere1_node = std::make_shared<SceneNode>(sphere1);
  std::shared_ptr<SceneNode> sphere2_node = std::make_shared<SceneNode>(sphere2);
  std::shared_ptr<SceneNode> cube_node = std::make_shared<SceneNode>(cube);
  std::shared_ptr<SceneNode> text_node = std::make_shared<SceneNode>(text->getModel());
  std::shared_ptr<SceneNode> quad_node = std::make_shared<SceneNode>(quad);

  bool forward = true;

  void setup() override {
    std::cout << "Setup" << std::endl;

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

    text_node->scale(0.2f, 0.2f, 0.2f);
    text_node->translate(0.0f, 0.2f, 0.0f);

    quad_node->translate(0.8f, 0.8f, 0.0f);
    quad_node->scale(0.1f, 0.1f, 0.1f);

    root_node->updateTransformation();

    cube1_node->setGrabbable(true);
    floor_node->setIsTerrain(true);
    cube_node->setIsTerrain(true);
  };

  void updateSimulation(XrTime predicted_time) override {
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
  };

  void draw(RenderContext& ctx) override {
    root_node->render(ctx);

    if (cube1_node->intersects(cube2_node)) {
      cube1->setColor({1.0f, 0.0f, 0.0f});
    } else {
      cube1->resetColor();
    }

    line->render(ctx);
  };
};

int main(int, char**) {
  VulkanApp application = VulkanApp("Test App");

  application.run();

  return 0;
};
