#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// OpenXR includes
#define XR_USE_GRAPHICS_API_VULKAN
#include <open_xr/openxr.h>
#include <open_xr/openxr_platform.h>

// GLM includes
#include <glm/glm/vec3.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

// XRe includes
#include <xre/utils.h>
#include <xre/structs.h>
#include <xre/buffer.h>

// Other includes
#include <vector>
#include <cstring>
#include <vector>
#include <string>
#include <set>
#include <functional>

class VulkanHandler {
public:
  VulkanHandler();
  VulkanHandler(XrInstance xr_instance, XrSystemId xr_system_id);

  void setupRenderer();
  void renderFrame(glm::mat4 view, glm::mat4 projection, VkFramebuffer framebuf, VkExtent2D resolution, std::function<void()> draw_callback);
  // void renderFrame(XrCompositionLayerProjectionView& view, std::function<void()> draw_callback,
  //                  glm::vec3 current_origin, Swapchain swapchain, uint32_t swapchain_image_id, uint32_t image_index);

  VkInstance getInstance();
  VkPhysicalDevice getPhysicalDevice();
  VkDevice getLogicalDevice();
  uint32_t getQueueFamilyIndex();
  VkRenderPass getRenderPass();

  static constexpr VkFormat s_color_format = VK_FORMAT_R8G8B8A8_SRGB;
private:
  // -------------------------------------------
  // Methods
  // -------------------------------------------
  VkShaderModule createShaderModule(const std::vector<char>& code);

  // -------------------------------------------
  // Attributes
  // -------------------------------------------
  // Vulkan instance
  VkInstance m_vk_instance = VK_NULL_HANDLE;

  // Logical device to use
  VkDevice m_device = VK_NULL_HANDLE;

  // The physical device we'll be using
  VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

  // Index of the queue family we use
  uint32_t m_queue_family_index = 0u;

  // Render pass for the graphics pipeline
  VkRenderPass m_render_pass = nullptr;

  // Graphics queue used for rendering
  VkQueue m_graphics_queue = nullptr;
  Buffer *m_uniform_buffer = nullptr;
  Buffer *m_vertex_buffer = nullptr;
  Buffer *m_index_buffer = nullptr;

  // Specifies the types of resources that are going to be accessed by the pipeline
  VkDescriptorSetLayout m_descriptor_set_layout = nullptr;
  VkDescriptorSet m_descriptor_set = nullptr;

  // Layout of the graphics pipeline (holds `uniform` values in shaders)
  VkPipelineLayout m_pipeline_layout = nullptr;
  VkPipeline m_graphics_pipeline = nullptr;

  // Command pool for our application (manage the memory that is used to store
  // command buffers).
  VkCommandPool m_command_pool = nullptr;

  // Command buffer for our application
  VkCommandBuffer m_command_buffer = nullptr;

  // Synchronization object
  VkFence m_fence = nullptr;
};
