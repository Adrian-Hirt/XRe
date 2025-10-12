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
#include <array>
#include <string>
#include <set>
#include <functional>

class VulkanHandler {
public:
  VulkanHandler();
  VulkanHandler(XrInstance xr_instance, XrSystemId xr_system_id);

  void setupRenderer();
  // void initializeVulkanAndDevices(XrInstance xr_instance, XrSystemId xr_system_id);
  // void createRenderPass(VkFormat swapchain_format);
  // void createDescriptorSetLayout();
  // VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
  // VkImageView createDepthImage(VkFormat format, uint32_t width, uint32_t height);
  // void createFramebuffers(Swapchain swapchain);
  // void createGraphicsPipeline(uint32_t viewport_height, uint32_t viewport_width);
  // void createCommandPool();
  // void createCommandBuffers();
  // void createSyncObjects();
  void render(glm::mat4 view, glm::mat4 projection, VkFramebuffer framebuf, VkExtent2D resolution);
  // void renderFrame(XrCompositionLayerProjectionView& view, std::function<void()> draw_callback,
  //                  glm::vec3 current_origin, Swapchain swapchain, uint32_t swapchain_image_id, uint32_t image_index);

  // VkInstance getInstance();
  // VkPhysicalDevice getPhysicalDevice();
  // VkDevice getLogicalDevice();
  // uint32_t getQueueFamilyIndex();

  static constexpr VkFormat s_color_format = VK_FORMAT_R8G8B8A8_SRGB;

  VkInstance m_vk_instance = VK_NULL_HANDLE;
  VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
  VkDevice m_device = nullptr;
  uint32_t m_queue_family_index = 0u;
  VkQueue m_graphics_queue = nullptr;
  VkRenderPass m_render_pass = nullptr;
  VkDescriptorSetLayout m_descriptor_set_layout = nullptr;
  VkDescriptorSet m_descriptor_set = nullptr;
  VkPipelineLayout m_pipeline_layout = nullptr;
  VkPipeline m_graphics_pipeline = nullptr;
  VkCommandPool m_command_pool = nullptr;
  VkCommandBuffer m_command_buffer = nullptr;
  VkFence m_fence = nullptr;
  
  Buffer *m_uniform_buffer = nullptr;
  Buffer *m_vertex_buffer = nullptr;

private:
  VkShaderModule createShaderModule(const std::vector<char>& code);

  // TODO: remove later
  static constexpr std::array vertices_ren = {
    Vertex({ -20.0f, 0.0f, -20.0f }, { 1.0f, 0.0f, 0.0f }), Vertex({ +20.0f, 0.0f, -20.0f }, { 0.0f, 1.0f, 0.0f }),
    Vertex({ -20.0f, 0.0f, +20.0f }, { 0.0f, 0.0f, 1.0f }), Vertex({ -20.0f, 0.0f, +20.0f }, { 0.0f, 0.0f, 1.0f }),
    Vertex({ +20.0f, 0.0f, -20.0f }, { 0.0f, 1.0f, 0.0f }), Vertex({ +20.0f, 0.0f, +20.0f }, { 1.0f, 0.0f, 1.0f })
  };

  struct UniformBufferObject final {
    glm::mat4 world;
    glm::mat4 view;
    glm::mat4 projection;
  } ubo;


  // -------------------------------------------
  // Methods
  // -------------------------------------------
  // void createInstance(XrInstance xr_instance, XrSystemId xr_system_id);
  // void setupDebugMessenger();
  // bool checkValidationLayerSupport();
  // void setupDevice(XrInstance xr_instance, XrSystemId xr_system_id);
  // uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
  // void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
  //                  VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
  // VkShaderModule createShaderModule(const std::vector<char>& code);


  // // -------------------------------------------
  // // Attributes
  // // -------------------------------------------
  // // Vulkan instance
  // VkInstance m_vk_instance;

  // // Debug messenger (which only prints message that have a certain level)
  // VkDebugUtilsMessengerEXT m_debug_messenger;

  // // The physical device we'll be using
  // VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;

  // // Logical device to use
  // VkDevice m_device;

  // // Graphics queue used for rendering
  // VkQueue m_graphics_queue;

  // // Queue used for presenting the rendered images
  // VkQueue m_present_queue;

  // // Swapchain
  // VkFormat m_swapchain_format;

  // // Render pass for the graphics pipeline
  // VkRenderPass m_render_pass;

  // // Specifies the types of resources that are going to be accessed by the pipeline
  // VkDescriptorSetLayout m_descriptor_set_layout;

  // // Size of the viewport
  // VkExtent2D m_viewport_extent;

  // // Layout of the graphics pipeline (holds `uniform` values in shaders)
  // VkPipelineLayout m_pipeline_layout;

  // // The graphics pipeline itself
  // VkPipeline m_graphics_pipeline;

  // // Array to hold the frame buffers for our swapchains
  // std::vector<VkFramebuffer> m_swapchain_framebuffers;

  // // Indices of the queue families we use
  // QueueFamilyIndices m_queue_family_indices;

  // // Command pool for our application (manage the memory that is used to store
  // // command buffers).
  // VkCommandPool m_command_pool;

  // // Command buffers for our application
  // std::vector<VkCommandBuffer> m_command_buffers;

  // // Synchronization objects
  // std::vector<VkSemaphore> m_image_available_semaphores;
  // std::vector<VkSemaphore> m_render_finished_semaphores;
  // std::vector<VkFence> m_in_flight_fences;

  // // Whether to enable or disable validation layers
  // #ifdef NULL
  // const static bool s_enable_validation_layers = false;
  // #else
  // const static bool s_enable_validation_layers = true;
  // #endif

  // // Allow 2 frames in flight
  // const static int s_max_frames_in_flight = 2;
};
