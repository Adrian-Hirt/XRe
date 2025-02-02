#include <xre/vulkan_handler.h>

// TODO: why can't i put this in a class?
// Validation layers we wish to use
const std::vector<const char*> s_validation_layers = {
  "VK_LAYER_KHRONOS_validation"
};

VulkanHandler::VulkanHandler() {};

VkInstance VulkanHandler::getInstance() {
  return m_vk_instance;
}

VkPhysicalDevice VulkanHandler::getPhysicalDevice() {
  return m_physical_device;
}

VkDevice VulkanHandler::getLogicalDevice() {
  return m_device;
}

void VulkanHandler::initializeVulkanAndDevices(XrInstance xr_instance, XrSystemId xr_system_id) {
  createInstance(xr_instance, xr_system_id);
  // setupDebugMessenger(); TODO: this seems broken :(
  setupDevice(xr_instance, xr_system_id);
}

void VulkanHandler::createInstance(XrInstance xr_instance, XrSystemId xr_system_id) {
  XrResult result;

  // Setup the application information to create the instance later on
  VkApplicationInfo vulkan_app_info{};
  vulkan_app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  vulkan_app_info.pApplicationName = "XRe Application"; // TODO
  vulkan_app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  vulkan_app_info.pEngineName = "XRe";
  vulkan_app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  vulkan_app_info.apiVersion = VK_API_VERSION_1_0;

  // Next, setup the info struct to create the vulkan instance. First,
  // add some basic data.
  VkInstanceCreateInfo instance_create_info{};
  instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instance_create_info.pApplicationInfo = &vulkan_app_info;

  // Get the required extensions from the OpenXR runtime
  PFN_xrGetVulkanInstanceExtensionsKHR ext_getVulkanInstanceExtensionsKHR;
  result = xrGetInstanceProcAddr(xr_instance, "xrGetVulkanInstanceExtensionsKHR", (PFN_xrVoidFunction*)(&ext_getVulkanInstanceExtensionsKHR));
  Utils::checkXrResult(result, "Failed to get the xrGetVulkanInstanceExtensionsKHR function pointer");

  uint32_t vulkan_instance_extensions_count;
  result = ext_getVulkanInstanceExtensionsKHR(xr_instance, xr_system_id, 0, &vulkan_instance_extensions_count, nullptr);
  Utils::checkXrResult(result, "Failed to get the count of the required Vulkan instance extensions");

  std::string buffer;
  buffer.resize(vulkan_instance_extensions_count);
  result = ext_getVulkanInstanceExtensionsKHR(xr_instance, xr_system_id, vulkan_instance_extensions_count, &vulkan_instance_extensions_count, buffer.data());
  Utils::checkXrResult(result, "Failed to get the required Vulkan instance extensions");

  std::vector<const char*> enabled_extensions = Utils::splitString(buffer, ' ');

  if (s_enable_validation_layers) {
    enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  instance_create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
  instance_create_info.ppEnabledExtensionNames = enabled_extensions.data();

  // Create the vulkan instance
  VkResult vk_result = vkCreateInstance(&instance_create_info, nullptr, &m_vk_instance);
  Utils::checkVkResult(vk_result, "failed to create instance!");
}

// Check that all of the validation layers we want are available
bool VulkanHandler::checkValidationLayerSupport() {
  // First get the count of layers
  uint32_t layer_count;
  vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

  // Then create a vector and call the method again to get all the available validation
  // layers that we may use.
  std::vector<VkLayerProperties> available_layers(layer_count);
  vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

  // Loop over all the layers we requested and see whether they are available.
  for (const std::string layer_name : s_validation_layers) {
    bool layer_found = false;

    for (const auto& layer_properties : available_layers) {
      if (strcmp(layer_name.c_str(), layer_properties.layerName) == 0) {
        layer_found = true;
        break;
      }
    }

    if (!layer_found) {
      return false;
    }
  }

  return true;
}

// Callback function used to filter out debug messages that are not important enough
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
  VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
  VkDebugUtilsMessageTypeFlagsEXT messageType,
  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
  void* pUserData
) {
  // Only show the message if it's important enough to show
  if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
  }

  return VK_FALSE;
}

// Setup the debug messenger if needed
void VulkanHandler::setupDebugMessenger() {
  if (!s_enable_validation_layers) {
    return;
  }

  // Create struct to create the debug messenger
  VkDebugUtilsMessengerCreateInfoEXT create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  create_info.pfnUserCallback = debugCallback;
  create_info.pUserData = nullptr; // Optional

  auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_vk_instance, "vkCreateDebugUtilsMessengerEXT");
  if (func(m_vk_instance, &create_info, nullptr, &m_debug_messenger) != VK_SUCCESS) {
    Utils::exitWithMessage("failed to set up debug messenger!");
  }
}

void VulkanHandler::setupDevice(XrInstance xr_instance, XrSystemId xr_system_id) {
  VkResult result;
  XrResult xr_result;

  PFN_xrGetVulkanGraphicsDeviceKHR xrGetVulkanGraphicsDeviceKHR;
	xr_result = xrGetInstanceProcAddr(xr_instance, "xrGetVulkanGraphicsDeviceKHR", (PFN_xrVoidFunction*)(&xrGetVulkanGraphicsDeviceKHR));
  Utils::checkXrResult(xr_result, "Failed to retrieve the `xrGetVulkanGraphicsDeviceKHR` function");

  xr_result = xrGetVulkanGraphicsDeviceKHR(xr_instance, xr_system_id, m_vk_instance, &m_physical_device);
  Utils::checkXrResult(xr_result, "Failed to retrieve the physical device for Vulkan");

  // Next, we need to setup the logical device.
  // Get the number of queue families
  uint32_t physical_device_queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &physical_device_queue_family_count, nullptr);

  // Retrieve the actual queue families.
  std::vector<VkQueueFamilyProperties> queue_families(physical_device_queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &physical_device_queue_family_count, queue_families.data());

  // Loop over all queue families and keep track of the queue families we are interested in.
  // We simply assume that the graphics queue family and the present graphics family are the same for now.
  // TODO: cleanup code for case where graphics family != present family. I'll need to figure out how to
  // check for surface support.
  int i = 0;
  for (const auto& queue_family : queue_families) {
    // Keep track of the graphics queue family
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      m_queue_family_indices.m_graphics_family = i;
      m_queue_family_indices.m_present_family = i;
      break;
    }

    i++;
  }

  if (!m_queue_family_indices.isComplete()) {
    Utils::exitWithMessage("Failed to find a graphics queue family");
  }

  PFN_xrGetVulkanDeviceExtensionsKHR ext_xrGetVulkanDeviceExtensionsKHR;
	xr_result = xrGetInstanceProcAddr(xr_instance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction*)(&ext_xrGetVulkanDeviceExtensionsKHR));
  Utils::checkXrResult(xr_result, "Failed to retrieve the `xrGetVulkanDeviceExtensionsKHR` function");

  // Get required Vulkan device extensions from OpenXR
  uint32_t vk_device_extensions_count;
  xr_result = ext_xrGetVulkanDeviceExtensionsKHR(xr_instance, xr_system_id, 0u, &vk_device_extensions_count, nullptr);
  Utils::checkXrResult(xr_result, "Failed to enumerate required Vulkan device extensions");

  std::string buffer;
  buffer.resize(vk_device_extensions_count);
  xr_result = ext_xrGetVulkanDeviceExtensionsKHR(xr_instance, xr_system_id, vk_device_extensions_count, &vk_device_extensions_count, buffer.data());
  Utils::checkXrResult(xr_result, "Failed to retrieve required Vulkan device extensions");

  std::vector<const char*> vulkanDeviceExtensions = Utils::splitString(buffer, ' ');

  // Create device
  float queue_priority = 1.0f;

  VkDeviceQueueCreateInfo queue_create_info{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
  queue_create_info.queueFamilyIndex = m_queue_family_indices.m_graphics_family.value();
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &queue_priority;

  // Specify used device features
  VkPhysicalDeviceFeatures device_features{};
  device_features.samplerAnisotropy = VK_TRUE;
  device_features.shaderStorageImageMultisample = VK_TRUE;

  // Verify that required physical device features are supported
  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(m_physical_device, &features);
  if (!features.shaderStorageImageMultisample) {
    Utils::exitWithMessage("Required Vulkan physical device feature \"Shader Storage Image Multisample\" not supported");
  }

  if(!features.samplerAnisotropy) {
    Utils::exitWithMessage("Required Vulkan physical device feature \"samplerAnisotropy\" not supported");
  }

  // Create the logical device
  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  // add infos about the device queues we want to create
  device_create_info.queueCreateInfoCount = 1;
  device_create_info.pQueueCreateInfos = &queue_create_info;

  // And add infos about the device features we need
  device_create_info.pEnabledFeatures = &device_features;

  // Setup the extensions we wish to use
  device_create_info.enabledExtensionCount = static_cast<uint32_t>(vulkanDeviceExtensions.size());
  device_create_info.ppEnabledExtensionNames = vulkanDeviceExtensions.data();

  // create the device
  result = vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device);
  Utils::checkVkResult(result, "failed to create logical device!");

  // and finally retrieve the created graphics queue
  vkGetDeviceQueue(m_device, m_queue_family_indices.m_graphics_family.value(), 0, &m_graphics_queue);
  vkGetDeviceQueue(m_device, m_queue_family_indices.m_present_family.value(), 0, &m_present_queue);
};

void VulkanHandler::createRenderPass(VkFormat swapchain_format) {
  m_swapchain_format = swapchain_format;

  // We only have a single color buffer attachment (represented by one of the images from the swapchain)
  VkAttachmentDescription color_attachment{};
  color_attachment.format = swapchain_format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  // Setup a reference to the color attachment we created beforehand
  VkAttachmentReference color_attachment_reference{};
  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Retrieve the depth attachment format
  std::vector<VkFormat> depth_attachment_format_candidates = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
  VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
  VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
  VkFormat found_format = VK_FORMAT_UNDEFINED;

  for (VkFormat format : depth_attachment_format_candidates) {
    VkFormatProperties properties;
    vkGetPhysicalDeviceFormatProperties(m_physical_device, format, &properties);

    if (tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & features) == features) {
      found_format = format;
    }
    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features) {
      found_format = format;
    }
  }

  if (found_format == VK_FORMAT_UNDEFINED) {
    Utils::exitWithMessage("Failed to find a valid depth attachment format!");
  }

  // And we have a single depth buffer attachment
  VkAttachmentDescription depth_attachment{};
  depth_attachment.format = found_format;
  depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // Setup the reference to the depth attachment
  VkAttachmentReference depth_attachment_reference{};
  depth_attachment_reference.attachment = 1;
  depth_attachment_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  // We're only using a single render pass
  VkSubpassDescription subpass{};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &color_attachment_reference;
  subpass.pDepthStencilAttachment = &depth_attachment_reference;

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
  dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

  // Create the render pass
  std::array<VkAttachmentDescription, 2> attachments = { color_attachment, depth_attachment };
  VkRenderPassCreateInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
  renderPassInfo.pAttachments = attachments.data();
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  VkResult result = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_render_pass);
  Utils::checkVkResult(result, "Failed to create render pass");
}

void VulkanHandler::createDescriptorSetLayout() {
  // Uniform buffer object
  VkDescriptorSetLayoutBinding ubo_layout_binding{};
  ubo_layout_binding.binding = 0;
  ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  ubo_layout_binding.descriptorCount = 1;
  ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  // Combined image sampler
  VkDescriptorSetLayoutBinding sampler_layout_binding{};
  sampler_layout_binding.binding = 1;
  sampler_layout_binding.descriptorCount = 1;
  sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  sampler_layout_binding.pImmutableSamplers = nullptr;
  sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  std::array<VkDescriptorSetLayoutBinding, 2> bindings = { ubo_layout_binding, sampler_layout_binding };

  VkDescriptorSetLayoutCreateInfo layout_create_info{};
  layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
  layout_create_info.pBindings = bindings.data();

  VkResult result = vkCreateDescriptorSetLayout(m_device, &layout_create_info, nullptr, &m_descriptor_set_layout);
  Utils::checkVkResult(result, "Failed to create the descriptor set layout");
}

uint32_t VulkanHandler::findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties) {
  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(m_physical_device, &memory_properties);

  for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++) {
    if ((type_filter & (1 << i)) && (memory_properties.memoryTypes[i].propertyFlags & properties) == properties) {
      return i;
    }
  }

  Utils::exitWithMessage("Failed to find a memory type");
  return 0; // To silence warnings, code is not reachable as statement above exits
}

void VulkanHandler::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
                               VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& image_memory) {
  VkResult result;

  // Create the image
  VkImageCreateInfo image_create_info{};
  image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  image_create_info.imageType = VK_IMAGE_TYPE_2D;
  image_create_info.extent.width = width;
  image_create_info.extent.height = height;
  image_create_info.extent.depth = 1;
  image_create_info.mipLevels = 1;
  image_create_info.arrayLayers = 1;
  image_create_info.format = format;
  image_create_info.tiling = tiling;
  image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  image_create_info.usage = usage;
  image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
  image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  result = vkCreateImage(m_device, &image_create_info, nullptr, &image);
  Utils::checkVkResult(result, "Failed to create the image");

  // Allocate memory for the image
  VkMemoryRequirements memory_requirements;
  vkGetImageMemoryRequirements(m_device, image, &memory_requirements);

  VkMemoryAllocateInfo memory_allocate_info{};
  memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  memory_allocate_info.allocationSize = memory_requirements.size;
  memory_allocate_info.memoryTypeIndex = findMemoryType(memory_requirements.memoryTypeBits, properties);

  result = vkAllocateMemory(m_device, &memory_allocate_info, nullptr, &image_memory);
  Utils::checkVkResult(result, "Failed to allocate memory for image");

  vkBindImageMemory(m_device, image, image_memory, 0);
}

VkImageView VulkanHandler::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {
  VkImageViewCreateInfo image_view_create_info{};
  image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  image_view_create_info.image = image;
  image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D; // Treat image as 2D texture
  image_view_create_info.format = format;

  // Keep the default mappings of the RGBA channels
  image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
  image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

  // subresourceRange describes the image’s purpose and which part of the
  // image should be accessed. The images should be used as color targets
  // without any mipmapping levels or multiple layers.
  // If the application was stereographic 3D, the swapchain would have multiple
  // layers representing the views for left and right.
  image_view_create_info.subresourceRange.aspectMask = aspectFlags;
  image_view_create_info.subresourceRange.baseMipLevel = 0;
  image_view_create_info.subresourceRange.levelCount = 1;
  image_view_create_info.subresourceRange.baseArrayLayer = 0;
  image_view_create_info.subresourceRange.layerCount = 1;

  VkImageView image_view;
  VkResult result = vkCreateImageView(m_device, &image_view_create_info, nullptr, &image_view);
  Utils::checkVkResult(result, "Failed to create image view");

  return image_view;
}

VkImageView VulkanHandler::createDepthImage(VkFormat format, uint32_t width, uint32_t height) {
  VkImage depth_image;
  VkDeviceMemory depth_image_memory;
  VkImageView depth_image_view;

  // Create the depth image
  createImage(width, height, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
              VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depth_image, depth_image_memory);

  // Create the image view
  depth_image_view = createImageView(depth_image, format, VK_IMAGE_ASPECT_DEPTH_BIT);

  return depth_image_view;
}

VkShaderModule VulkanHandler::createShaderModule(const std::vector<char>& code) {
  // Setup the create info for the shader module
  VkShaderModuleCreateInfo shader_module_create_info{};
  shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  shader_module_create_info.codeSize = code.size();
  shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

  // And create the shader module itself
  VkShaderModule shader_module;
  VkResult result = vkCreateShaderModule(m_device, &shader_module_create_info, nullptr, &shader_module);
  Utils::checkVkResult(result, "Failed to create shader module");

  return shader_module;
}

void VulkanHandler::createGraphicsPipeline(uint32_t viewport_height, uint32_t viewport_width) {
  m_viewport_extent = {};
  m_viewport_extent.height = viewport_height;
  m_viewport_extent.width = viewport_width;

  // --------------------------
  // Shader modules
  // --------------------------
  // Load shader bytecodes
  static std::vector<char> vertex_shader_code = Utils::readFile(SHADERS_FOLDER "/vert.spv");
  static std::vector<char> fragment_shader_code = Utils::readFile(SHADERS_FOLDER "/frag.spv");

  // Create shader modules for both of the shaders
  VkShaderModule vertex_shader_module = createShaderModule(vertex_shader_code);
  VkShaderModule fragment_shader_module = createShaderModule(fragment_shader_code);

  // We need to assign the shaders to specific pipeline stages, start with the vertext shader
  VkPipelineShaderStageCreateInfo vertex_shader_stage_create_info{};
  vertex_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertex_shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertex_shader_stage_create_info.module = vertex_shader_module;
  vertex_shader_stage_create_info.pName = "main";

  // and do the same for the fragment shader
  VkPipelineShaderStageCreateInfo fragment_shader_stage_create_info{};
  fragment_shader_stage_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragment_shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragment_shader_stage_create_info.module = fragment_shader_module;
  fragment_shader_stage_create_info.pName = "main";

  VkPipelineShaderStageCreateInfo shader_stages[] = { vertex_shader_stage_create_info, fragment_shader_stage_create_info };

  // --------------------------
  // Dynamic state
  // --------------------------
  VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
  dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_create_info.dynamicStateCount = 0;
  dynamic_state_create_info.pDynamicStates = nullptr;

  // --------------------------
  // Vertex input
  // --------------------------
  auto vertex_binding_description = Vertex::getBindingDescription();
  auto vertex_attribute_descriptions = Vertex::getAttributeDescriptions();
  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attribute_descriptions.size());
  vertex_input_info.pVertexBindingDescriptions = &vertex_binding_description;
  vertex_input_info.pVertexAttributeDescriptions = vertex_attribute_descriptions.data();

  // --------------------------
  // Input assembly
  // --------------------------
  // Describe the input topology and if primitive restart is enabled
  VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
  input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_info.primitiveRestartEnable = VK_FALSE;

  // --------------------------
  // Viewports and scissors
  // --------------------------
  // Setup the viewport
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = (float) m_viewport_extent.width;
  viewport.height = (float) m_viewport_extent.height;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;

  // We're not using a scissor rectangle (i.e. draw the whole viewport)
  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = m_viewport_extent;

  // Setup the viewport state
  VkPipelineViewportStateCreateInfo viewport_state_info{};
  viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_info.viewportCount = 1;
  viewport_state_info.pViewports = &viewport;
  viewport_state_info.scissorCount = 1;
  viewport_state_info.pScissors = &scissor;

  // --------------------------
  // Rasterizer
  // --------------------------
  // Setup the rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  // --------------------------
  // Multisampling
  // --------------------------
  // Keep multisampling disabled for now
  VkPipelineMultisampleStateCreateInfo multisampling_info{};
  multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_info.sampleShadingEnable = VK_FALSE;
  multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  // --------------------------
  // Depth buffer and stencil
  // --------------------------
  VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
  depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_info.depthTestEnable = VK_TRUE;
  depth_stencil_info.depthWriteEnable = VK_TRUE;
  depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
  depth_stencil_info.stencilTestEnable = VK_FALSE;

  // --------------------------
  // Color blending
  // --------------------------
  VkPipelineColorBlendAttachmentState color_blend_attachment_state{};
  color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment_state.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo color_blend_info{};
  color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_info.logicOpEnable = VK_FALSE;
  color_blend_info.attachmentCount = 1;
  color_blend_info.pAttachments = &color_blend_attachment_state;

  // --------------------------
  // Pipeline layout
  // --------------------------
  VkPipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = 1;
  pipeline_layout_info.pSetLayouts = &m_descriptor_set_layout;

  VkResult result = vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout);
  Utils::checkVkResult(result, "Failed to create pipeline layout");

  // --------------------------
  // Graphics pipeline
  // --------------------------
  VkGraphicsPipelineCreateInfo pipeline_create_info{};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.stageCount = 2;
  pipeline_create_info.pStages = shader_stages;
  pipeline_create_info.pVertexInputState = &vertex_input_info;
  pipeline_create_info.pInputAssemblyState = &input_assembly_info;
  pipeline_create_info.pViewportState = &viewport_state_info;
  pipeline_create_info.pRasterizationState = &rasterizer;
  pipeline_create_info.pMultisampleState = &multisampling_info;
  pipeline_create_info.pDepthStencilState = nullptr; // Optional
  pipeline_create_info.pColorBlendState = &color_blend_info;
  pipeline_create_info.pDynamicState = &dynamic_state_create_info;
  pipeline_create_info.pDepthStencilState = &depth_stencil_info;
  pipeline_create_info.layout = m_pipeline_layout;
  pipeline_create_info.renderPass = m_render_pass;
  pipeline_create_info.subpass = 0;

  result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &m_graphics_pipeline);
  Utils::checkVkResult(result, "Failed to create the graphics pipeline");


  // Make sure to cleanup the shader modules
  vkDestroyShaderModule(m_device, fragment_shader_module, nullptr);
  vkDestroyShaderModule(m_device, vertex_shader_module, nullptr);
}

void VulkanHandler::createFramebuffers(Swapchain swapchain) {
  m_swapchain_framebuffers.resize(swapchain.image_views.size());

  // For each image views from the swapchain, we need to create a framebuffer
  for (size_t i = 0; i < swapchain.image_views.size(); i++) {
    std::array<VkImageView, 2> attachments = {
      swapchain.image_views[i],
      swapchain.depth_image_view
    };

    VkFramebufferCreateInfo framebuffer_create_info{};
    framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebuffer_create_info.renderPass = m_render_pass;
    framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebuffer_create_info.pAttachments = attachments.data();
    framebuffer_create_info.width = m_viewport_extent.width;
    framebuffer_create_info.height = m_viewport_extent.height;
    framebuffer_create_info.layers = 1;

    VkResult result = vkCreateFramebuffer(m_device, &framebuffer_create_info, nullptr, &m_swapchain_framebuffers[i]);
    Utils::checkVkResult(result, "Failed to create framebuffer");
  }
}

void VulkanHandler::createCommandPool() {
  VkCommandPoolCreateInfo pool_create_info{};
  pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  pool_create_info.queueFamilyIndex = m_queue_family_indices.m_graphics_family.value();

  VkResult result = vkCreateCommandPool(m_device, &pool_create_info, nullptr, &m_command_pool);
  Utils::checkVkResult(result, "Failed to create the command pool");
}

void VulkanHandler::createCommandBuffers() {
  m_command_buffers.resize(s_max_frames_in_flight);

  VkCommandBufferAllocateInfo buffer_allocate_info{};
  buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  buffer_allocate_info.commandPool = m_command_pool;
  buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  buffer_allocate_info.commandBufferCount = (uint32_t) m_command_buffers.size();

  VkResult result = vkAllocateCommandBuffers(m_device, &buffer_allocate_info, m_command_buffers.data());
  Utils::checkVkResult(result, "Failed to allocate command buffers");
}

void VulkanHandler::createSyncObjects() {
  m_image_available_semaphores.resize(s_max_frames_in_flight);
  m_render_finished_semaphores.resize(s_max_frames_in_flight);
  m_in_flight_fences.resize(s_max_frames_in_flight);

  VkSemaphoreCreateInfo semaphore_create_info{};
  semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fence_create_info{};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  // Create the fence in the signalled state such that the first `vkWaitForFences` call
  // does not block indefinitely.
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  VkResult result;

  for (size_t i = 0; i < s_max_frames_in_flight; i++) {
    result = vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_image_available_semaphores[i]);
    Utils::checkVkResult(result, "Failed to create semaphore");

    result = vkCreateSemaphore(m_device, &semaphore_create_info, nullptr, &m_render_finished_semaphores[i]);
    Utils::checkVkResult(result, "Failed to create semaphore");

    result = vkCreateFence(m_device, &fence_create_info, nullptr, &m_in_flight_fences[i]);
    Utils::checkVkResult(result, "Failed to create fence");
  }
}

uint32_t VulkanHandler::getQueueFamilyIndex() {
  return m_queue_family_indices.m_graphics_family.value();
}
