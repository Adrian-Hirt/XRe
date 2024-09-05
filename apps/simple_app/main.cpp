#include <xre/application.h>

class TestApp : public Application {
public:
  TestApp(const char *application_name) : Application(application_name) {};

  Model floor = ModelFactory::createGroundPlane(10, DATA_FOLDER "/textures/Tiles012_2K-JPG_Color.jpg");
  Model cube = ModelFactory::createCube({0.0f, 1.0f, 0.0f, 1.0f});
  Model transparent_cube = ModelFactory::createCube({1.0f, 0.0f, 0.0f, 0.25f});
  Model brick_cube = ModelFactory::createCube(DATA_FOLDER "/textures/Bricks090_2K-JPG_Color.jpg");

  // Create custom models
  Model sphere = Model(DATA_FOLDER "/models/sphere.obj");

  // Create a line
  Line line = Line({0.0f, 0.0f, 0.0f}, {10.0f, 10.0f, 5.0f}, {1.0f, 0.0f, 0.0f, 1.0f});

  // Create bitmaps
  Bitmap quad = Bitmap(0.70f, 0.95f, 0.25, 0.25, DATA_FOLDER "/textures/MetalWalkway013_2K-PNG_Color_Opacity.png");

  // Create some text with some valid and some invalid characters
  Text text = Text("This is a sample text :) カタカナ");

  // Build the scene nodes
  SceneNode root_node = SceneNode();
  SceneNode floor_node = SceneNode(&floor, root_node);
  SceneNode spinning_cube_node = SceneNode(&cube, root_node);
  SceneNode moon_cube_node = SceneNode(&cube, spinning_cube_node);
  SceneNode transparent_cube_node = SceneNode(&transparent_cube, root_node);
  SceneNode sphere_1_node = SceneNode(&sphere, root_node);
  SceneNode sphere_2_node = SceneNode(&sphere, root_node);
  SceneNode sphere_3_node = SceneNode(&sphere, root_node);
  SceneNode text_node = SceneNode(&text, root_node);
  SceneNode quad_node = SceneNode(&quad, root_node);
  SceneNode line_node = SceneNode(&line, root_node);
  SceneNode brick_cube_node = SceneNode(&brick_cube, root_node);

  void setup() override {
    spinning_cube_node.scale(0.3f, 0.3f, 0.3f);
    spinning_cube_node.translate(1.0f, 1.0f, 1.0f);
    spinning_cube_node.setGrabbable(true);

    moon_cube_node.translate(2.0f, 1.0f, 0.0f);
    moon_cube_node.scale(0.1f, 0.1f, 0.1f);

    transparent_cube.m_has_transparency = true;
    transparent_cube_node.scale(0.1f, 0.1f, 0.1f);
    transparent_cube_node.translate(2.0f, 1.5f, 0.0f);
    transparent_cube_node.setGrabbable(true);

    sphere_1_node.scale(0.1f, 0.1f, 0.1f);

    sphere_2_node.scale(0.1f, 0.1f, 0.1f);
    sphere_2_node.translate(1.0f, 0.0f, 2.0f);

    sphere_3_node.scale(0.1f, 0.1f, 0.1f);
    sphere_3_node.translate(-2.0f, 0.0f, -2.5f);

    // Set the ground to be terrain
    floor_node.setIsTerrain(true);

    brick_cube_node.translate(4.0f, 0.6f, -3.0f);
    brick_cube_node.scale(1.0f, 1.2f, 0.8f);
    brick_cube_node.setGrabbable(true);
  };

  void updateSimulation(XrTime predicted_time) override {
    // Rotate the cube
    // TODO: we should interpolate the rotation value such that it's
    // not coupled to the framerate
    spinning_cube_node.rotate(0.0f, 0.0f, 0.01f);
    root_node.updateTransformation();
  };

  void draw() override {
    root_node.render();
  };
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
};
