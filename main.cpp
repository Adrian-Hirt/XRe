#include <xre/xre_application.h>

class TestApp : public XReApplication {
public:
  TestApp(const char *application_name) : XReApplication(application_name) {};

  Shader shader = Shader(SHADERS_FOLDER "/triangle.hlsl", get_device(), get_device_context());

  void draw(XrCompositionLayerProjectionView &view) override {
    shader.render(get_device_context());
  }
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
}
