#include <xre/xre_application.h>

int main(int, char**) {
  XReApplication application = XReApplication("Test App");

  application.run();

  return 0;
}
