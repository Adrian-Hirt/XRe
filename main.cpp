#include <xre/xre_application.h>

class TestApp : public XReApplication {
public:
  TestApp(const char *application_name) : XReApplication(application_name) {};

  void draw(XrCompositionLayerProjectionView &view) override {
    std::cout << "Hello from Testapp" << std::endl;
  }
};

int main(int, char**) {
  TestApp application = TestApp("Test App");

  application.run();

  return 0;
}
