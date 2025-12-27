#include <xre/texture.h>

Texture::Texture(const std::string& path) {

}

void Texture::registerVulkanHandler(VulkanHandler handler) {
  s_vulkan_handler = handler;
}
