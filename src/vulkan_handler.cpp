#include <xre/vulkan_handler.h>

// TODO: why can't i put this in a class?
// Validation layers we wish to use
const std::vector<const char*> validation_layers = {
  "VK_LAYER_KHRONOS_validation"
};

VulkanHandler::VulkanHandler() {
  createInstance();
  setupDebugMessenger();
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
    instance_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
    instance_create_info.ppEnabledLayerNames = validation_layers.data();
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
  for (const std::string layer_name : validation_layers) {
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
    throw std::runtime_error("failed to set up debug messenger!");
  }
}
