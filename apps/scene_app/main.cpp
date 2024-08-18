#include <xre/application.h>

class SceneApp : public Application {
public:
  SceneApp(const char *application_name) : Application(application_name) {};

  void setup() override {

  };

  void updateSimulation(XrTime predicted_time) override {

  };

  void draw() override {

  };
};

int main(int, char**) {
  SceneApp application = SceneApp("Scene App");

  application.run();

  return 0;
};
