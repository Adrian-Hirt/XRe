#include <xre/application.h>
#include <xre/object_oriented_bounding_box.h>

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};

  Model cube1 = ModelFactory::createCube({0.0f, 1.0f, 0.0f});
  Model cube2 = ModelFactory::createCube({0.0f, 1.0f, 0.0f});
  // Model sphere = ModelFactory::createSphere();

  Model floor = ModelFactory::createGroundPlane(10);
  Model cube = ModelFactory::createCube({0.0f, 1.0f, 0.0f});
  Line line = Line(0.003f, 2.0f, {1.0f, 0.0f, 0.0f});
  XrTime last_time = 0;

  SceneNode root_node = SceneNode();
  SceneNode floor_node = SceneNode(&floor);
  SceneNode cube1_node = SceneNode(&cube1);
  SceneNode cube2_node = SceneNode(&cube2);
  // SceneNode sphere_node = SceneNode(&sphere);

  bool forward = true;

  void setup() override {
    std::cout << "Setup" << std::endl;

    // Setup scene graph
    root_node.addChildNode(&cube1_node);
    root_node.addChildNode(&cube2_node);
    root_node.addChildNode(&floor_node);
    // root_node.addChildNode(&sphere_node);

    // Update initial transforms
    cube1_node.setScale(0.1f, 0.1f, 0.1f);
    cube2_node.setScale(0.1f, 0.1f, 0.1f);
    cube1_node.setPosition(0.0f, 1.5f, -2.0f);
    cube2_node.setPosition(0.0f, 1.5f, 0.0f);
    // sphere_node.scale(0.1f, 0.1f, 0.1f);
    // sphere_node.setPosition(0.0f, 1.5f, 0.0f);
    root_node.updateTransformation();

    cube1.toggleRenderBoundingBoxes();
    cube2.toggleRenderBoundingBoxes();

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
