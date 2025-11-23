#include <xre/application.h>

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};

  Model floor = ModelFactory::createGroundPlane(10);
  Model cube = ModelFactory::createCube({0.0f, 1.0f, 0.0f});
  Line line = Line(0.003f, 2.0f, {1.0f, 0.0f, 0.0f});
  XrTime last_time = 0;

  SceneNode root_node = SceneNode();
  SceneNode floor_node = SceneNode(&floor);
  SceneNode cube_node = SceneNode(&cube);

  void setup() override {
    std::cout << "Setup" << std::endl;

    // Setup scene graph
    root_node.addChildNode(&cube_node);
    root_node.addChildNode(&floor_node);

    // Update initial transforms
    cube_node.setScale(0.1f, 0.1f, 0.1f);
    cube_node.setPosition(0.0f, 1.5f, -2.0f);
    root_node.updateTransformation();
  };

  void updateSimulation(XrTime predicted_time) override {
    // compute delta time in seconds
    float dt = 0.0f;

    // skip first frame
    if (last_time != 0) {
      dt = static_cast<float>(predicted_time - last_time) * 1e-9f; // assuming XrTime is in ns
    }
    last_time = predicted_time;

    // rotation speed in degrees per second
    constexpr float rotation_speed = 45.0f;
    float angle = rotation_speed * dt;

    // apply rotation around Y axis
    glm::quat delta = glm::angleAxis(glm::radians(angle), glm::vec3(0,1,0));
    auto rotation = glm::normalize(delta * cube_node.getRotation());
    cube_node.setRotation(rotation);

    // Update transformation
    root_node.updateTransformation();
  };

  void draw(RenderContext& ctx) override {
    // std::cout << "Draw" << std::endl;
    root_node.render(ctx);

    line.render(ctx);
  };
};

int main(int, char**) {
  VulkanApp application = VulkanApp("Test App");

  application.run();

  return 0;
};
