#include <xre/application.h>

class TestApp : public Application {
public:
  TestApp(const char *application_name) : Application(application_name) {};

  Shader shader = Shader(SHADERS_FOLDER "/triangle.hlsl", getDevice(), getDeviceContext());
  Model cube_model = model_factory.createCube();

  void setup() override {
    shader.activate();
  }

  void draw(XrCompositionLayerProjectionView &view) override {
    // Rotate the cube
    cube_model.rotate(0.0f, 0.0f, 0.01f);

    // Compute the viewProjectionMatrix as well as the model matrix
    // and send them to the GPU
    shader.setViewProjectionMatrix(Geometry::computeViewProjectionMatrix(view));
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
