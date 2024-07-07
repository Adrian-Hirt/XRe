#include <xre/application.h>

class TestApp : public Application {
public:
  TestApp(const char *application_name) : Application(application_name) {};

  // Create shaders
  Shader ambient_shader = Shader::loadOrCreate(SHADERS_FOLDER "/ambient.hlsl");
  Shader color_shader = Shader::loadOrCreate(SHADERS_FOLDER "/color.hlsl");
  Shader texture_shader = Shader::loadOrCreate(SHADERS_FOLDER "/ambient_texture.hlsl");

  // Create models with the ModelFactory
  Model cube = ModelFactory::createCube({1.0f, 0.0f, 0.0f, 0.25f});
  Model ground_cube = ModelFactory::createCube(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
  Model ground = ModelFactory::createGroundPlane(10, DATA_FOLDER "/textures/Tiles012_2K-JPG_Color.jpg");

  // Create custom models
  Model sphere = Model(DATA_FOLDER "/models/sphere.obj");

  // Create a line
  Line line = Line({0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 5.0f}, {1.0f, 0.0f, 0.0f, 1.0f});

  // Create bitmaps
  Bitmap quad = Bitmap(0.70f, 0.95f, 0.25, 0.25, DATA_FOLDER "/textures/MetalWalkway013_2K-PNG_Color_Opacity.png");

  // Create some text with some valid and some invalid characters
  Text text = Text("This is a sample text :) カタカナ");

  void setup() override {
    // Scale the cube down a bit
    cube.scale(0.33f, 0.33f, 0.33f);

    // And translate the cube up a bit
    cube.translate(0.0f, 4.0f, 0.0f);
    cube.makeInteractable();

    sphere.makeInteractable();

    // Translate the ground cube up a bit and to the left
    ground_cube.translate(-2.0f, 0.5f, 0.0f);
    ground_cube.makeInteractable();

    // Squish the ground cube
    ground_cube.scale(1.0f, 0.3f, 1.0f);
  };

  void updateSimulation(XrTime predicted_time) override {
    // Rotate the cube
    // TODO: we should interpolate the rotation value such that it's
    // not coupled to the framerate
    cube.rotate(0.0f, 0.0f, 0.01f);
  };

  void draw() override {
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

    quad.render();

    text.render();
  };
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
};
