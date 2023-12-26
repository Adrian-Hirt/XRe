#include <xre/application.h>

class TestApp : public Application {
public:
  TestApp(const char *application_name) : Application(application_name) {};

  // Create shaders
  Shader ambient_shader = Shader(SHADERS_FOLDER "/ambient.hlsl");
  Shader color_shader = Shader(SHADERS_FOLDER "/color.hlsl");
  Shader texture_shader = Shader(SHADERS_FOLDER "/ambient_texture.hlsl");

  // Create models with the model_factors
  Model cube = model_factory.createCube({1.0f, 0.0f, 0.0f, 0.25f});
  Model ground_cube = model_factory.createCube(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
  Model ground = model_factory.createGroundPlane(10, DATA_FOLDER "/textures/Tiles012_2K-JPG_Color.jpg");

  // Create custom models
  Model sphere = Model(DATA_FOLDER "/models/sphere.obj");

  // Create a line
  Line line = Line({0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 5.0f}, {1.0f, 0.0f, 0.0f, 1.0f});

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

    // Render the ground
    ground.render(&texture_shader);

    // Render the sphere
    sphere.render(&color_shader);

    // Render the cube at the ground
    ground_cube.render(&ambient_shader);

    // Render the line
    line.render(&color_shader);

    // Render the cube model twice, once with Counterclockwise
    // cull mode, once with the normal clockwise cull mode, such
    // that the transparency works correctly.
    setCcwCullMode();
    cube.render(&ambient_shader);
    setCwCullMode();
    cube.render(&ambient_shader);

  }
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
}
