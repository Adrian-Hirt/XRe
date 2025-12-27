#include <xre/application.h>

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};

  Texture texture = resourceManager()->texture(DATA_FOLDER "textures/Wood048_2K-JPG_Color.jpg");

  Material material = Material(SHADERS_FOLDER "vk/ambient.vert.spv", SHADERS_FOLDER "vk/basic.frag.spv");
  Material basic_material = Material(SHADERS_FOLDER "vk/basic.vert.spv", SHADERS_FOLDER "vk/basic.frag.spv");
  Material texture_material = Material(SHADERS_FOLDER "vk/texture.vert.spv", SHADERS_FOLDER "vk/texture.frag.spv", texture);

  Model cube1 = ModelFactory::createCube({0.0f, 1.0f, 0.0f}, basic_material);
  Model cube2 = ModelFactory::createCube({0.0f, 1.0f, 0.0f}, material);
  Model sphere1 = ModelFactory::createSphere(material);
  Model sphere2 = ModelFactory::createSphere(material);

  Model floor = ModelFactory::createGroundPlane(10, texture_material);
  Model cube = ModelFactory::createCube({0.0f, 1.0f, 0.0f}, material);
  Line line = Line(0.003f, 2.0f, {1.0f, 0.0f, 0.0f}, material);
  XrTime last_time = 0;

  SceneNode root_node = SceneNode();
  SceneNode floor_node = SceneNode(&floor);
  SceneNode cube1_node = SceneNode(&cube1);
  SceneNode cube2_node = SceneNode(&cube2);
  SceneNode sphere1_node = SceneNode(&sphere1);
  SceneNode sphere2_node = SceneNode(&sphere2);

  bool forward = true;

  void setup() override {
    std::cout << "Setup" << std::endl;

    sphere1.setColor({1.0f, 0.0f, 0.0f});
    sphere2.setColor({1.0f, 0.0f, 0.0f});
    floor.setColor({0.7f, 0.7f, 0.7f});

    // Setup scene graph
    root_node.addChildNode(&cube1_node);
    root_node.addChildNode(&cube2_node);
    root_node.addChildNode(&floor_node);
    root_node.addChildNode(&sphere1_node);
    root_node.addChildNode(&sphere2_node);

    // Update initial transforms
    cube1_node.setScale(0.1f, 0.1f, 0.1f);
    cube2_node.setScale(0.1f, 0.1f, 0.1f);
    cube1_node.setPosition(0.0f, 1.5f, -2.0f);
    cube2_node.setPosition(0.0f, 1.5f, 0.0f);
    sphere1_node.scale(0.1f, 0.1f, 0.1f);
    sphere2_node.scale(0.1f, 0.1f, 0.1f);
    sphere2_node.setPosition(5.0f, 0.0f, -4.0f);
    root_node.updateTransformation();

    cube1.toggleRenderBoundingBoxes();
    cube2.toggleRenderBoundingBoxes();
    // sphere.toggleRenderBoundingBoxes();

    cube1_node.setGrabbable(true);
    // cube2_node.setGrabbable(true);
    floor_node.setIsTerrain(true);

    // cube1.printBouindingBoxes();
  };

  void updateSimulation(XrTime predicted_time) override {
  //   // compute delta time in seconds
  //   float dt = 0.0f;

  //   // skip first frame
  //   if (last_time != 0) {
  //     dt = static_cast<float>(predicted_time - last_time) * 1e-9f; // assuming XrTime is in ns
  //   }
  //   last_time = predicted_time;

  //   // rotation speed in degrees per second
  //   constexpr float rotation_speed = 45.0f;
  //   float angle = rotation_speed * dt;

  //   // apply rotation around Y axis
  //   glm::quat delta = glm::angleAxis(glm::radians(angle), glm::vec3(0,1,0));
  //   auto rotation = glm::normalize(delta * cube_node.getRotation());
  //   cube_node.setRotation(rotation);

    if (forward) {
      cube2_node.translate(0.0f, 0.0f, -0.01f);
    } else {
      cube2_node.translate(0.0f, 0.0f, 0.01f);
    }

    float z = cube2_node.getPosition()[2];

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
    root_node.updateTransformation();
  };

  void draw(RenderContext& ctx) override {
    // std::cout << "Draw" << std::endl;
    root_node.render(ctx);

    if (cube1_node.intersects(cube2_node)) {
      cube1.setColor({1.0f, 0.0f, 0.0f});
    } else {
      cube1.resetColor();
    }

    line.render(ctx);
  };
};

int main(int, char**) {
  VulkanApp application = VulkanApp("Test App");

  application.run();

  return 0;
};
