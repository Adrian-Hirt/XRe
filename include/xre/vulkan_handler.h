#pragma once

// Vulkan includes
#include <vulkan/vulkan.h>

// OpenXR includes
#define XR_USE_GRAPHICS_API_VULKAN
#include <open_xr/openxr.h>
#include <open_xr/openxr_platform.h>

// GLM includes
#include <glm/glm/vec3.hpp>

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
  VulkanHandler(XrInstance xr_instance, XrSystemId xr_system_id, const char *application_name);

  void setupRenderer();
  void renderFrame(glm::mat4 view, glm::mat4 projection, VkFramebuffer framebuf, VkExtent2D resolution,
                   std::function<void(RenderContext &)> draw_callback, std::function<void(RenderContext &)> draw_interactions_callback);

  VkInstance getInstance();
  VkPhysicalDevice getPhysicalDevice();
  VkDevice getLogicalDevice();
  uint32_t getQueueFamilyIndex();
  VkRenderPass getRenderPass();

  static constexpr VkFormat s_color_format = VK_FORMAT_R8G8B8A8_SRGB;
  static constexpr uint32_t s_max_models_in_scene = 256;
  static constexpr uint32_t s_max_descriptors = 20; // TODO: we might need to be able to handle more materials

  VkPipelineLayout createPipelineLayout();
  VkPipeline createGraphicsPipeline(const std::string &vert_path, const std::string &frag_path);
  void bindGraphicsPipeline(VkPipeline pipeline);
  Buffer *createUniformBuffer();
  VkDescriptorSet allocateDescriptorSet(Buffer *material_uniform_buffer, VkImageView texture_image_view, VkSampler texture_sampler);

  VkCommandBuffer beginSingleTimeCommands();
  void endSingleTimeCommands(VkCommandBuffer commandBuffer);

private:
  // -------------------------------------------
  // Methods
  // -------------------------------------------
  VkShaderModule createShaderModule(const std::vector<char> &code);

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

  // Uniform buffers
  Buffer *m_uniform_buffer = nullptr;
  Buffer *m_global_uniform_buffer = nullptr;

  // Vertex and index buffers
  Buffer *m_vertex_buffer = nullptr;
  Buffer *m_index_buffer = nullptr;

  // Specifies the types of resources that are going to be accessed by the pipeline
  VkDescriptorSetLayout m_descriptor_set_layout = nullptr;
  VkDescriptorPool m_local_descriptor_pool;

  VkDescriptorSetLayout m_global_descriptor_set_layout = nullptr;
  VkDescriptorSet m_global_descriptor_set = nullptr;

  // Layout of the graphics pipeline (holds `uniform` values in shaders)
  VkPipelineLayout m_pipeline_layout = nullptr;

  // Command pool for our application (manage the memory that is used to store
  // command buffers).
  VkCommandPool m_command_pool = nullptr;

  // Command buffer for our application
  VkCommandBuffer m_command_buffer = nullptr;

  // Synchronization object
  VkFence m_fence = nullptr;

  // Aligned size for the uniform buffer
  VkDeviceSize m_aligned_size;
};
