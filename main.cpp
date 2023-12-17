#include <xre/application.h>

class TestApp : public Application {
public:
  TestApp(const char *application_name) : Application(application_name) {};

  Shader shader = Shader(SHADERS_FOLDER "/triangle.hlsl", getDevice(), getDeviceContext());
  Model cube_model;

  void setup() override {
    shader.activate();

    std::vector<vertex> cube_vertices {
      {-1.0f, -1.0f,  1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      { 1.0f, -1.0f,  1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-1.0f,  1.0f,  1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      { 1.0f,  1.0f,  1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-1.0f, -1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      {-1.0f,  1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      { 1.0f, -1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
      { 1.0f,  1.0f, -1.0f, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
    };

    std::vector<unsigned int> cube_indices{
      2, 1, 0,    // side 1
      3, 1, 2,
      6, 5, 4,    // side 2
      7, 5, 6,
      7, 2, 5,    // side 3
      3, 2, 7,
      0, 6, 4,    // side 4
      1, 6, 0,
      1, 7, 6,    // side 5
      3, 7, 1,
      4, 5, 2,    // side 6
      2, 0, 4
    };

    Mesh cube_mesh = Mesh(getDevice(), getDeviceContext(), cube_vertices, cube_indices);
    cube_model = Model(getDevice(), getDeviceContext(), { cube_mesh });
  }

  void draw(XrCompositionLayerProjectionView &view) override {
    // Rotate the cube
    cube_model.rotate(0.0f, 0.0f, 0.01f);

    // Compute the viewProjectionMatrix as well as the model matrix
    // and send them to the GPU
    shader.setViewProjectionMatrix(computeViewProjectionMatrix(view));
    shader.setModelMatrix(cube_model.getTransformationMatrix());
    shader.updateConstantBuffer();

    // Render the cube model
    cube_model.render();
  }
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
}
