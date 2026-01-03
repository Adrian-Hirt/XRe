#include <xre/application.h>

// Include scenes
#include "scenes/main_scene.h"
#include "scenes/other_scene.h"

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};

  std::shared_ptr<ResourceManager> res = resourceManager();

  void setup() override {
    SceneManager::instance().registerScene("main", [this] {
      return std::make_unique<MainScene>(res);
    });

    SceneManager::instance().registerScene("other", [this] {
      return std::make_unique<OtherScene>(res);
    });

    SceneManager::instance().setActive("main");
  }
};

int main(int, char**) {
  VulkanApp application = VulkanApp("Test App");

  application.run();

  return 0;
};
