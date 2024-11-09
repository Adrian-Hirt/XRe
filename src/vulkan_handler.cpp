#include <xre/vulkan_handler.h>

// TODO: why can't i put this in a class?
// Validation layers we wish to use
const std::vector<const char*> s_validation_layers = {
  "VK_LAYER_KHRONOS_validation"
};

// Extensions for the device we wish to use
const std::vector<const char*> s_device_extensions = {
  VK_KHR_SWAPCHAIN_EXTENSION_NAME
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
  createInstance();
  // setupDebugMessenger(); TODO: this seems broken :(
  setupDevice(xr_instance, xr_system_id);
}

void VulkanHandler::createInstance() {
  VkResult result;

  // First, check if all of our validation layers we want to use are available
  if (s_enable_validation_layers && !checkValidationLayerSupport()) {
    Utils::exitWithMessage("validation layers requested, but not available!");
  }

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

  // We simply assume we're only going to need these extensions for now.
  std::vector<const char*> enabled_extensions = { VK_KHR_SURFACE_EXTENSION_NAME };
  if (s_enable_validation_layers) {
    enabled_extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
  }

  instance_create_info.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
  instance_create_info.ppEnabledExtensionNames = enabled_extensions.data();

  // If validation layers are enabled, add them to the create info.
  if (s_enable_validation_layers) {
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(s_validation_layers.size());
    instance_create_info.ppEnabledLayerNames = s_validation_layers.data();
  }
  else {
    instance_create_info.enabledLayerCount = 0;
  }

  // Create the vulkan instance
  result = vkCreateInstance(&instance_create_info, nullptr, &m_vk_instance);
  Utils::checkVkResult(result, "failed to create instance!");
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
  QueueFamilyIndices queue_family_indices;
  int i = 0;
  for (const auto& queue_family : queue_families) {
    // Keep track of the graphics queue family
    if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queue_family_indices.m_graphics_family = i;
      queue_family_indices.m_present_family = i;
      break;
    }

    i++;
  }

  if (!queue_family_indices.isComplete()) {
    Utils::exitWithMessage("Failed to find a graphics queue family");
  }

  // Create device queues for the present and the graphics families
  std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
  // TODO: currently this uses only a single queue family for the graphics and present
  // queue, we probably need to adapt this to also support a present family.
  std::set<uint32_t> queue_family_indices_set = {
    queue_family_indices.m_graphics_family.value(),
    queue_family_indices.m_present_family.value()
  };

  float queue_priority = 1.0f;
  for (uint32_t queue_family : queue_family_indices_set) {
    VkDeviceQueueCreateInfo queue_create_info{};
    queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queue_create_info.queueFamilyIndex = queue_family;
    queue_create_info.queueCount = 1;
    queue_create_info.pQueuePriorities = &queue_priority;
    queue_create_infos.push_back(queue_create_info);
  }

  // Specify used device features
  VkPhysicalDeviceFeatures device_features{};
  device_features.samplerAnisotropy = VK_TRUE;

  // Create the logical device
  VkDeviceCreateInfo device_create_info{};
  device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

  // add infos about the device queues we want to create
  device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
  device_create_info.pQueueCreateInfos = queue_create_infos.data();

  // And add infos about the device features we need
  device_create_info.pEnabledFeatures = &device_features;

  // Setup the extensions we wish to use
  device_create_info.enabledExtensionCount = static_cast<uint32_t>(s_device_extensions.size());
  device_create_info.ppEnabledExtensionNames = s_device_extensions.data();

  // Obsolete code which is only put in for compability with older vulkan implementations
  if (s_enable_validation_layers) {
    device_create_info.enabledLayerCount = static_cast<uint32_t>(s_validation_layers.size());
    device_create_info.ppEnabledLayerNames = s_validation_layers.data();
  }
  else {
    device_create_info.enabledLayerCount = 0;
  }

  // create the device
  result = vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device);
  Utils::checkVkResult(result, "failed to create logical device!");

  // and finally retrieve the created graphics queue
  vkGetDeviceQueue(m_device, queue_family_indices.m_graphics_family.value(), 0, &m_graphics_queue);
  vkGetDeviceQueue(m_device, queue_family_indices.m_present_family.value(), 0, &m_present_queue);
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
