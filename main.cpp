#include <xre/xre_application.h>

class TestApp : public XReApplication {
public:
  TestApp(const char *application_name) : XReApplication(application_name) {};

  Shader shader = Shader(SHADERS_FOLDER "/triangle.hlsl", get_device(), get_device_context());

  void setup() override {
    vertex triangle_vertices[] = {
      {0.0f, 0.5f, 0.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {0.45f, -0.5, 0.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
      {-0.45f, -0.5f, 0.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}
    };

    shader.setVertexData(triangle_vertices, sizeof(triangle_vertices));

    shader.activate();
  }

  void draw(XrCompositionLayerProjectionView &view) override {
    shader.render();
  }
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
}
