#include <xre/application.h>

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};


  void setup() override {
    std::cout << "Setup" << std::endl;
  };

  // void updateSimulation(XrTime predicted_time) override {

  // };

  void draw() override {

  };
};

int main(int, char**) {
  VulkanApp application = VulkanApp("Test App");

  application.run();

  return 0;
};
