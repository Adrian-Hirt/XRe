#include <xre/application.h>

class SceneApp : public Application {
public:
  SceneApp(const char *application_name) : Application(application_name) {};

  Model cube = ModelFactory::createCube({0.56f, 0.93f, 0.56f, 1.0f});
  Model moon_cube = ModelFactory::createCube({1.0f, 1.0f, 0.0f, 1.0f});
  Model floor = ModelFactory::createGroundPlane(10, DATA_FOLDER "/textures/Tiles012_2K-JPG_Color.jpg");
  SceneNode root_node = SceneNode();
  SceneNode floor_node = SceneNode(&floor);
  SceneNode cube_node = SceneNode(&cube);
  SceneNode moon_node = SceneNode(&moon_cube);

  void setup() override {
    root_node.addChildNode(cube_node);
    cube_node.addChildNode(moon_node);
    root_node.addChildNode(floor_node);
    moon_node.translate(2.0f, 1.0f, 0.0f);
    moon_node.scale(0.1f, 0.1f, 0.1f);
    root_node.updateTransformation();
    cube_node.setGrabbable(true);
    floor_node.setIsTerrain(true);
  };

  void updateSimulation(XrTime predicted_time) override {
    cube_node.rotate(0.0f, 0.0f, 0.01f);
    root_node.updateTransformation();
  };

  void draw() override {
    // TODO: do not consider bounding boxes if model is not rendered for intersection!
    root_node.render();
  };
};

int main(int, char**) {
  SceneApp application = SceneApp("Scene App");

  application.run();

  return 0;
};
