#include <xre/application.h>

class TestApp : public Application {
public:
  TestApp(const char *application_name) : Application(application_name) {};

  Shader ambient_shader = Shader(SHADERS_FOLDER "/ambient.hlsl", getDevice(), getDeviceContext());
  Shader color_shader = Shader(SHADERS_FOLDER "/color.hlsl", getDevice(), getDeviceContext());
  Shader texture_shader = Shader(SHADERS_FOLDER "/texture.hlsl", getDevice(), getDeviceContext());
  Model cube = model_factory.createCube();
  Model ground_cube = model_factory.createCube(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
  Model ground = model_factory.createGroundPlane(10, DATA_FOLDER "/textures/wood.jpg");
  Model sphere = Model(getDevice(), getDeviceContext(), DATA_FOLDER "/models/sphere.obj");
  Line line = Line(getDevice(), getDeviceContext(), {0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 5.0f}, {1.0f, 0.0f, 0.0f, 1.0f});

  void setup() override {
    // Scale the cube down a bit
    cube.scale(0.33f, 0.33f, 0.33f);

    // And translate the cube up a bit
    cube.translate(0.0f, 4.0f, 0.0f);

    // Translate the ground cube up a bit and to the left
    ground_cube.translate(-2.0f, 0.5f, 0.0f);

    // Squish the ground cube
    ground_cube.scale(1.0f, 0.3f, 1.0f);
  }

  void draw(XrCompositionLayerProjectionView &view) override {
    // Rotate the cube
    cube.rotate(0.0f, 0.0f, 0.01f);

    // Render the cube model
    cube.render(&ambient_shader);

    // Render the cube at the ground
    ground_cube.render(&color_shader);

    // Render the ground
    ground.render(&texture_shader);

    // Render the sphere
    sphere.render(&color_shader);

    // Render the line
    line.render(&color_shader);
  }
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
}
