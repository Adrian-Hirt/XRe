#include <xre/application.h>

class TestApp : public Application {
public:
  TestApp(const char *application_name) : Application(application_name) {};

  Shader shader = Shader(SHADERS_FOLDER "/triangle.hlsl", getDevice(), getDeviceContext());
  Model cube_model = model_factory.createCube();
  Model ground = model_factory.createGround();

  void setup() override {
    // Activate the shader
    shader.activate();

    // Scale the cube down a bit
    cube_model.scale(0.33f, 0.33f, 0.33f);

    // And translate the cube up a bit
    cube_model.translate(0.0f, 4.0f, 0.0f);
  }

  void draw(XrCompositionLayerProjectionView &view) override {
    // Rotate the cube
    cube_model.rotate(0.0f, 0.0f, 0.01f);

    // Compute the viewProjectionMatrix as well as the model matrix
    // and send them to the GPU
    // TODO: this should probably happen before the draw method in
    // the openxr handler, and set a static value in the shader class
    // which is shader among the shaders.
    shader.setViewProjectionMatrix(Geometry::computeViewProjectionMatrix(view));
    shader.updateConstantBuffer();

    // Render the cube model
    cube_model.render();

    // Render the ground
    ground.render();
  }
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
}
