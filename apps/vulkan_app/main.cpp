#include <xre/application.h>

// Include scenes
#include "scenes/main_scene.h"
#include "scenes/other_scene.h"

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};

  std::shared_ptr<ResourceManager> res = resourceManager();
  std::shared_ptr<SceneManager> scenes = sceneManager();

  void setup() override {
    scenes->registerScene("main", [this] {
      return std::make_unique<MainScene>(res, scenes);
    });

    scenes->registerScene("other", [this] {
      return std::make_unique<OtherScene>(res, scenes);
    });

    scenes->setActive("main");
  }
};

int main(int, char**) {
  VulkanApp application = VulkanApp("Test App");

  application.run();

  return 0;
};
