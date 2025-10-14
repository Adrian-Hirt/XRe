#include <xre/application.h>

class VulkanApp : public Application {
public:
  VulkanApp(const char *application_name) : Application(application_name) {};

  // Mesh *mesh = nullptr;
  Model *floor = nullptr;
  Model *square = nullptr;
  Model *sphere = nullptr;

  void setup() override {
    std::cout << "Setup" << std::endl;

    std::vector<Vertex> vertices = {
      Vertex({ -20.0f, 0.0f, -20.0f }, { 1.0f, 0.0f, 0.0f }), // 0
      Vertex({ +20.0f, 0.0f, -20.0f }, { 0.0f, 1.0f, 0.0f }), // 1
      Vertex({ -20.0f, 0.0f, +20.0f }, { 0.0f, 0.0f, 1.0f }), // 2
      Vertex({ +20.0f, 0.0f, +20.0f }, { 1.0f, 0.0f, 1.0f })  // 3
    };

    std::vector<uint16_t> indices = {
      0, 1, 2, // First triangle
      2, 1, 3  // Second triangle
    };

    Mesh *mesh = new Mesh(vertices, indices);
    std::vector<Mesh*> meshes = { mesh };
    floor = new Model(meshes);
    square = new Model(DATA_FOLDER "models/square.obj");
    sphere = new Model(DATA_FOLDER "models/sphere.obj");
  };

  void updateSimulation(XrTime predicted_time) override {
    // std::cout << "Update at " << predicted_time << std::endl;
  };

  void draw(RenderContext& ctx) override {
    // std::cout << "Draw" << std::endl;
    square->render(ctx);
    // sphere->render(ctx);
    // floor->render(ctx);
  };
};

int main(int, char**) {
  VulkanApp application = VulkanApp("Test App");

  application.run();

  return 0;
};
