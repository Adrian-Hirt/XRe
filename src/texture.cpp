#include <xre/texture.h>

#define STB_IMAGE_IMPLEMENTATION

Texture::Texture(const std::string& path) {

}

void Texture::registerVulkanHandler(VulkanHandler handler) {
  s_vulkan_handler = handler;
}
