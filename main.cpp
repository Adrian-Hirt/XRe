#include <xre/xre_application.h>

class TestApp : public XReApplication {
public:
  TestApp(const char *application_name) : XReApplication(application_name) {};

  Shader shader = Shader(SHADERS_FOLDER "/triangle.hlsl", get_device(), get_device_context());
  Mesh cube;

  void setup() override {
    shader.activate();

    std::vector<vertex> cube_vertices {
      {-0.2f,  0.2f, 0.2f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      { 0.2f,  0.2f, 0.2f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-0.2f, -0.2f, 0.2f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      { 0.2f, -0.2f, 0.2f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-0.2f,  0.2f, 0.6f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      { 0.2f,  0.2f, 0.6f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-0.2f, -0.2f, 0.6f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      { 0.2f, -0.2f, 0.6f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)}
    };


    std::vector<unsigned int> cube_indices{
      0, 1, 2,
      2, 1, 3,
      4, 0, 6,
      6, 0, 2,
      7, 5, 6,
      6, 5, 4,
      3, 1, 7,
      7, 1, 5,
      4, 5, 0,
      0, 5, 1,
      3, 7, 2,
      2, 7, 6,
    };

    cube = Mesh(get_device(), get_device_context(), cube_vertices, cube_indices);
  }

  void draw(XrCompositionLayerProjectionView &view) override {
    cube.render();
  }
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
}
