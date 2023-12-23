#include <xre/application.h>

class TestApp : public Application {
public:
  TestApp(const char *application_name) : Application(application_name) {};

  Shader ambient_shader = Shader(SHADERS_FOLDER "/ambient.hlsl", getDevice(), getDeviceContext());
  Shader color_shader = Shader(SHADERS_FOLDER "/color.hlsl", getDevice(), getDeviceContext());
  Model cube_model = model_factory.createCube();
  Model ground = model_factory.createGround();

  void setup() override {
    // Scale the cube down a bit
    cube_model.scale(0.33f, 0.33f, 0.33f);

    // And translate the cube up a bit
    cube_model.translate(0.0f, 4.0f, 0.0f);
  }

  void draw(XrCompositionLayerProjectionView &view) override {
    // Rotate the cube
    cube_model.rotate(0.0f, 0.0f, 0.01f);

    // Render the cube model
    cube_model.render(&color_shader);

    // Render the ground
    ground.render(&ambient_shader);
  }
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
}
