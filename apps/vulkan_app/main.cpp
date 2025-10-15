#include <xre/application.h>

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};

  Model floor = ModelFactory::createGroundPlane(10);
  Model cube = ModelFactory::createCube({0.0f, 1.0f, 0.0f});

  void setup() override {
    std::cout << "Setup" << std::endl;
  };

  void updateSimulation(XrTime predicted_time) override {
    // std::cout << "Update at " << predicted_time << std::endl;
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
