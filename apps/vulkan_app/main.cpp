#include <xre/application.h>

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};

  Model floor = ModelFactory::createGroundPlane(10);
  Model cube = ModelFactory::createCube({0.0f, 1.0f, 0.0f});
  XrTime last_time = 0;

  void setup() override {
    std::cout << "Setup" << std::endl;
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
    cube.m_rotation = glm::normalize(delta * cube.m_rotation);
  };

  void draw(RenderContext& ctx) override {
    // std::cout << "Draw" << std::endl;
    floor.render(ctx);
    cube.render(ctx);
  };
};

int main(int, char**) {
  VulkanApp application = VulkanApp("Test App");

  application.run();

  return 0;
};
