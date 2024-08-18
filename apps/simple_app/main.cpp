#include <xre/application.h>

class TestApp : public Application {
public:
  TestApp(const char *application_name) : Application(application_name) {};

  // Create shaders
  Shader ambient_shader = Shader::loadOrCreate(SHADERS_FOLDER "/ambient.hlsl");
  Shader color_shader = Shader::loadOrCreate(SHADERS_FOLDER "/color.hlsl");
  Shader texture_shader = Shader::loadOrCreate(SHADERS_FOLDER "/ambient_texture.hlsl");

  // Create models with the ModelFactory
  Model spinning_cube = ModelFactory::createCube({1.0f, 0.0f, 0.0f, 0.25f});
  Model ground_cube = ModelFactory::createCube(DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
  Model ground = ModelFactory::createGroundPlane(10, DATA_FOLDER "/textures/Tiles012_2K-JPG_Color.jpg");
  Model small_cube = ModelFactory::createCube(DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

  // Create custom models
  Model sphere = Model(DATA_FOLDER "/models/sphere.obj");
  Model sphere2 = Model(DATA_FOLDER "/models/sphere.obj");
  Model sphere3 = Model(DATA_FOLDER "/models/sphere.obj");

  // Create a line
  Line line = Line({0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 5.0f}, {1.0f, 0.0f, 0.0f, 1.0f});

  // Create bitmaps
  Bitmap quad = Bitmap(0.70f, 0.95f, 0.25, 0.25, DATA_FOLDER "/textures/MetalWalkway013_2K-PNG_Color_Opacity.png");

  // Create some text with some valid and some invalid characters
  Text text = Text("This is a sample text :) カタカナ");

  void setup() override {
    // Scale the cube down a bit
    spinning_cube.scale(0.33f, 0.33f, 0.33f);

    // And translate the cube up a bit
    spinning_cube.translate(0.0f, 4.0f, 0.0f);
    spinning_cube.setGrabbable(true);

    // Scale the small cube down and translate it a bit. Also, mark it as grabbable.
    small_cube.scale(0.1f, 0.1f, 0.1f);
    small_cube.translate(2.0f, 1.5f, 0.0f);
    small_cube.setGrabbable(true);

    sphere.scale(0.1f, 0.1f, 0.1f);
    sphere.setGrabbable(true);

    // Translate the ground cube up a bit and to the left
    ground_cube.translate(-2.0f, 0.5f, 0.0f);
    ground_cube.setGrabbable(true);

    // Squish the ground cube
    ground_cube.scale(1.0f, 0.3f, 1.0f);

    sphere2.scale(0.1f, 0.1f, 0.1f);
    sphere2.translate(1.0f, 0.0f, 2.0f);
    sphere2.setColor({1.0f, 0.0f, 0.0f, 1.0f});

    sphere3.scale(0.1f, 0.1f, 0.1f);
    sphere3.translate(-2.0f, 0.0f, -2.5f);
    sphere3.setColor({0.0f, 1.0f, 0.0f, 1.0f});

    // Set the ground to be terrain
    ground.setIsTerrain(true);
  };

  void updateSimulation(XrTime predicted_time) override {
    // Rotate the cube
    // TODO: we should interpolate the rotation value such that it's
    // not coupled to the framerate
    spinning_cube.rotate(0.0f, 0.0f, 0.01f);
  };

  void draw() override {
    // Render the ground
    ground.render(texture_shader);

    // Render the sphere
    sphere.render(color_shader);
    sphere2.render(color_shader);
    sphere3.render(color_shader);

    // Render the cube at the ground
    ground_cube.render(ambient_shader);

    // Render the line
    line.render(color_shader);

    // Render the small cube
    small_cube.render();

    // Render the flat bitmaps
    quad.render();
    text.render();

    // Render a transparent looking cube
    spinning_cube.renderTransparent(ambient_shader);
  };
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
};
