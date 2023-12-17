#include <xre/xre_application.h>

class TestApp : public XReApplication {
public:
  TestApp(const char *application_name) : XReApplication(application_name) {};

  Shader shader = Shader(SHADERS_FOLDER "/triangle.hlsl", get_device(), get_device_context());
  Mesh cube;

  void setup() override {
    shader.activate();

    std::vector<vertex> cube_vertices {
      {-1.0f, -1.0f, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, -1.0f, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-1.0f, 1.0f, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, 1.0f, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},

      {-1.0f, -1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-1.0f, 1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, -1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, 1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},

      {-1.0f, 1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-1.0f, 1.0f, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, 1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, 1.0f, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},

      {-1.0f, -1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, -1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-1.0f, -1.0f, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, -1.0f, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},

      {1.0f, -1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, 1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, -1.0f, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {1.0f, 1.0f, 1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},

      {-1.0f, -1.0f, -1.0f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
      {-1.0f, -1.0f, 1.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
      {-1.0f, 1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-1.0f, 1.0f, 1.0f, DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)}
    };

    std::vector<unsigned int> cube_indices{
      2, 1, 0,    // side 1
      3, 1, 2,
      6, 5, 4,    // side 2
      7, 5, 6,
      10, 9, 8,    // side 3
      11, 9, 10,
      14, 13, 12,    // side 4
      15, 13, 14,
      18, 17, 16,    // side 5
      19, 17, 18,
      20, 22, 23,    // side 6
      23, 21, 20
    };

    cube = Mesh(get_device(), get_device_context(), cube_vertices, cube_indices);
  }

  void draw(XrCompositionLayerProjectionView &view) override {
    shader.setViewProjectionMatrix(computeViewProjectionMatrix(view));
    shader.updateConstantBuffer();

    cube.render();
  }
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
}
