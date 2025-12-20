#include <xre/application.h>
#include <xre/object_oriented_bounding_box.h>

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};

  Model floor = ModelFactory::createGroundPlane(10);
  Model cube = ModelFactory::createCube({0.0f, 1.0f, 0.0f});
  Line line = Line(0.003f, 2.0f, {1.0f, 0.0f, 0.0f});
  XrTime last_time = 0;

  SceneNode root_node = SceneNode();
  SceneNode floor_node = SceneNode(&floor);
  SceneNode cube_node = SceneNode(&cube);

  void setup() override {
    std::cout << "Setup" << std::endl;

    // Setup scene graph
    root_node.addChildNode(&cube_node);
    root_node.addChildNode(&floor_node);

    // Update initial transforms
    cube_node.setScale(0.1f, 0.1f, 0.1f);
    cube_node.setPosition(0.0f, 1.5f, -2.0f);
    root_node.updateTransformation();
  };

  void updateSimulation(XrTime predicted_time) override {
    // compute delta time in seconds
    float dt = 0.0f;

    // skip first frame
    if (last_time != 0) {
      dt = static_cast<float>(predicted_time - last_time) * 1e-9f; // assuming XrTime is in ns
    }
    last_time = predicted_time;

    // rotation speed in degrees per second
    constexpr float rotation_speed = 45.0f;
    float angle = rotation_speed * dt;

    // apply rotation around Y axis
    glm::quat delta = glm::angleAxis(glm::radians(angle), glm::vec3(0,1,0));
    auto rotation = glm::normalize(delta * cube_node.getRotation());
    cube_node.setRotation(rotation);

    // Update transformation
    root_node.updateTransformation();
  };

  void draw(RenderContext& ctx) override {
    // std::cout << "Draw" << std::endl;
    root_node.render(ctx);

    line.render(ctx);
  };
};

int main(int, char**) {
  // VulkanApp application = VulkanApp("Test App");

  // application.run();

  // std::vector<glm::vec3> vertices = {
  //   {1,2,3}, {2,3,5}, {3,1,1}, {-1,0,2}, {4,2,0}
  // };

  std::vector<glm::vec3> vertices = {
    { 1.20f,  2.10f,  0.30f},
    { 2.15f,  2.85f,  0.55f},
    { 3.05f,  3.40f,  0.90f},
    { 4.10f,  4.20f,  1.25f},
    { 5.05f,  4.90f,  1.60f},

    { 1.60f,  1.30f,  1.10f},
    { 2.45f,  2.05f,  1.45f},
    { 3.30f,  2.80f,  1.80f},
    { 4.25f,  3.55f,  2.15f},
    { 5.10f,  4.30f,  2.50f},

    { 0.95f,  1.90f,  2.20f},
    { 1.80f,  2.65f,  2.55f},
    { 2.70f,  3.40f,  2.90f},
    { 3.65f,  4.15f,  3.25f},
    { 4.60f,  4.90f,  3.60f},

    { 1.10f,  0.85f,  0.40f},
    { 2.00f,  1.60f,  0.75f},
    { 2.95f,  2.35f,  1.10f},
    { 3.85f,  3.10f,  1.45f},
    { 4.80f,  3.85f,  1.80f},

    { 1.45f,  1.45f,  2.80f},
    { 2.35f,  2.20f,  3.15f},
    { 3.30f,  2.95f,  3.50f},
    { 4.20f,  3.70f,  3.85f},
    { 5.15f,  4.45f,  4.20f},

    { 0.70f,  1.25f,  1.60f},
    { 1.55f,  2.00f,  1.95f},
    { 2.45f,  2.75f,  2.30f},
    { 3.40f,  3.50f,  2.65f},
    { 4.35f,  4.25f,  3.00f}
  };


  OOBB box = OOBB(vertices);

  std::cout << "Center: " 
            << box.getCenter().x << ", " 
            << box.getCenter().y << ", " 
            << box.getCenter().z << "\n";

  std::cout << "Extents: "
            << box.getExtents().x << ", "
            << box.getExtents().y << ", "
            << box.getExtents().z << "\n";

  std::cout << "Axes: (columns)\n";

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      std::cout << box.getAxes()[i][j] << ", ";
    }
    std::cout << std::endl;
  }

  return 0;
};
