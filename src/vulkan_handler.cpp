#include <xre/vulkan_handler.h>

// // TODO: why can't i put this in a class?
// // Validation layers we wish to use
// const std::vector<const char*> s_validation_layers = {
//   "VK_LAYER_KHRONOS_validation"
// };

VulkanHandler::VulkanHandler() {};

VulkanHandler::VulkanHandler(XrInstance xr_instance, XrSystemId xr_system_id) {
  VkResult result;
  XrResult xr_result;

  //------------------------------------------------------------------------------------------------------
  // Retrieve the needed extensions
  //------------------------------------------------------------------------------------------------------
  PFN_xrGetVulkanInstanceExtensionsKHR getVulkanInstanceExtensionsKHR = nullptr;
  xr_result = xrGetInstanceProcAddr(xr_instance, "xrGetVulkanInstanceExtensionsKHR", reinterpret_cast<PFN_xrVoidFunction*>(&getVulkanInstanceExtensionsKHR));
  Utils::checkXrResult(xr_result, "Failed to retrieve xrGetVulkanInstanceExtensionsKHR function");

  // Get all supported Vulkan instance extensions
  std::vector<VkExtensionProperties> supportedVulkanInstanceExtensions;
  uint32_t instanceExtensionCount;
  result = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, nullptr);
  Utils::checkVkResult(result, "Failed to enumerate supported Vulkan instance extensions");

  if (instanceExtensionCount == 0u) {
    Utils::exitWithMessage("Found zero supported Vulkan instance extensions");
  }

  supportedVulkanInstanceExtensions.resize(instanceExtensionCount);
  result = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtensionCount, supportedVulkanInstanceExtensions.data());
  Utils::checkVkResult(result, "Failed to retrieve supported Vulkan instance extensions");

  // Get required Vulkan instance extensions from OpenXR
  uint32_t instance_extension_count;
  xr_result = getVulkanInstanceExtensionsKHR(xr_instance, xr_system_id, 0u, &instance_extension_count, nullptr);
  Utils::checkXrResult(xr_result, "Failed to get required Vulkan instance extensions");

  std::string instance_extension_buffer;
  instance_extension_buffer.resize(instance_extension_count);
  xr_result = getVulkanInstanceExtensionsKHR(xr_instance, xr_system_id, instance_extension_count, &instance_extension_count, instance_extension_buffer.data());
  Utils::checkXrResult(xr_result, "Failed to get required Vulkan instance extensions");

  std::vector<const char*> vulkan_instance_extensions = Utils::splitString(instance_extension_buffer, ' ');

  // Check that all Vulkan instance extensions are supported
  for (const char* extension : vulkan_instance_extensions) {
    bool extensionSupported = false;

    for (const VkExtensionProperties& supportedExtension : supportedVulkanInstanceExtensions) {
      if (strcmp(extension, supportedExtension.extensionName) == 0) {
        extensionSupported = true;
        break;
      }
    }

    if (!extensionSupported) {
      Utils::exitWithMessage("Required Vulkan instance extension not supported");
    }
  }

  //------------------------------------------------------------------------------------------------------
  // Vulkan instance
  //------------------------------------------------------------------------------------------------------
  // Create Vulkan instance
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
  instance_create_info.enabledExtensionCount = static_cast<uint32_t>(vulkan_instance_extensions.size());
  instance_create_info.ppEnabledExtensionNames = vulkan_instance_extensions.data();

  // Create the vulkan instance
  result = vkCreateInstance(&instance_create_info, nullptr, &m_vk_instance);
  Utils::checkVkResult(result, "Failed to create Vulkan instance");

  //------------------------------------------------------------------------------------------------------
  // Physical device
  //------------------------------------------------------------------------------------------------------
  PFN_xrGetVulkanGraphicsDeviceKHR getVulkanGraphicsDeviceKHR = nullptr;
  xr_result = xrGetInstanceProcAddr(xr_instance, "xrGetVulkanGraphicsDeviceKHR", reinterpret_cast<PFN_xrVoidFunction*>(&getVulkanGraphicsDeviceKHR));
  Utils::checkXrResult(xr_result, "Failed to retrieve xrGetVulkanGraphicsDeviceKHR function");

  // Retrieve physical device from OpenXR
  xr_result = getVulkanGraphicsDeviceKHR(xr_instance, xr_system_id, m_vk_instance, &m_physical_device);
  Utils::checkXrResult(xr_result, "Failed to retrieve physical device from OpenXR");

  //------------------------------------------------------------------------------------------------------
  // Logical device
  //------------------------------------------------------------------------------------------------------
  // Next, we need to setup the logical device.
  // Get the number of queue families
  uint32_t physical_device_queue_family_count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &physical_device_queue_family_count, nullptr);

  if (physical_device_queue_family_count == 0u) {
    Utils::exitWithMessage("Found zero queue families for physical device");
  }

  // Retrieve the actual queue families.
  std::vector<VkQueueFamilyProperties> queue_families(physical_device_queue_family_count);
  vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &physical_device_queue_family_count, queue_families.data());


  // Loop over all queue families and keep track of the queue families we are interested in.
  // We simply assume that the graphics queue family and the present graphics family are the same for now.
  bool queue_family_index_found = false;
  for (uint32_t i = 0; i < queue_families.size(); i++) {
    VkQueueFamilyProperties& candidate = queue_families[i];

    if (candidate.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      m_queue_family_index = i;
      queue_family_index_found = true;
      break;
    }
  }

  Utils::checkBoolResult(queue_family_index_found, "Failed to find graphics queue for physical device!");

  PFN_xrGetVulkanDeviceExtensionsKHR ext_xrGetVulkanDeviceExtensionsKHR;
	xr_result = xrGetInstanceProcAddr(xr_instance, "xrGetVulkanDeviceExtensionsKHR", (PFN_xrVoidFunction*)(&ext_xrGetVulkanDeviceExtensionsKHR));
  Utils::checkXrResult(xr_result, "Failed to retrieve the `xrGetVulkanDeviceExtensionsKHR` function");

  // Get all supported Vulkan device extensions
  std::vector<VkExtensionProperties> supportedVulkanDeviceExtensions;
  uint32_t deviceExtensionCount;
  result = vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &deviceExtensionCount, nullptr);
  Utils::checkVkResult(result, "Failed to enumerate supported Vulkan device extensions");

  if (deviceExtensionCount == 0u) {
    Utils::exitWithMessage("Found zero supported Vulkan device extensions");
  }

  supportedVulkanDeviceExtensions.resize(deviceExtensionCount);
  result = vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &deviceExtensionCount, supportedVulkanDeviceExtensions.data());
  Utils::checkVkResult(result, "Failed to retrieve supported Vulkan device extensions");

  // Get required Vulkan device extensions from OpenXR
  uint32_t vk_device_extensions_count;
  xr_result = ext_xrGetVulkanDeviceExtensionsKHR(xr_instance, xr_system_id, 0u, &vk_device_extensions_count, nullptr);
  Utils::checkXrResult(xr_result, "Failed to enumerate required Vulkan device extensions");

  std::string buffer;
  buffer.resize(vk_device_extensions_count);
  xr_result = ext_xrGetVulkanDeviceExtensionsKHR(xr_instance, xr_system_id, vk_device_extensions_count, &vk_device_extensions_count, buffer.data());
  Utils::checkXrResult(xr_result, "Failed to retrieve required Vulkan device extensions");

  std::vector<const char*> vulkanDeviceExtensions = Utils::splitString(buffer, ' ');


  // Check that all Vulkan device extensions are supported
  for (const char* extension : vulkanDeviceExtensions) {
    bool extensionSupported = false;
    for (const VkExtensionProperties& supportedExtension : supportedVulkanDeviceExtensions) {
      if (strcmp(extension, supportedExtension.extensionName) == 0) {
        extensionSupported = true;
        break;
      }
    }

    Utils::checkBoolResult(extensionSupported, "Required Vulkan device extensions not supported");
  }

  // Create device
  float queue_priority = 1.0f;

  VkDeviceQueueCreateInfo queue_create_info{VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO};
  queue_create_info.queueFamilyIndex = m_queue_family_index;
  queue_create_info.queueCount = 1;
  queue_create_info.pQueuePriorities = &queue_priority;

  // Specify used device features
  VkPhysicalDeviceFeatures device_features{};
  device_features.samplerAnisotropy = VK_TRUE;
  device_features.shaderStorageImageMultisample = VK_TRUE;

  VkPhysicalDeviceFeatures physicalDeviceFeatures{};
  physicalDeviceFeatures.shaderStorageImageMultisample = VK_TRUE; // Required for OpenXR (to avoid a validation error)

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
  
  // Check graphics requirements for Vulkan
  PFN_xrGetVulkanGraphicsRequirementsKHR getVulkanGraphicsRequirementsKHR = nullptr;
  xr_result = xrGetInstanceProcAddr(xr_instance, "xrGetVulkanGraphicsRequirementsKHR", reinterpret_cast<PFN_xrVoidFunction*>(&getVulkanGraphicsRequirementsKHR));
  Utils::checkXrResult(xr_result, "Failed to retrieve xrGetVulkanGraphicsRequirementsKHR function");

  XrGraphicsRequirementsVulkanKHR graphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN_KHR };
  xr_result = getVulkanGraphicsRequirementsKHR(xr_instance, xr_system_id, &graphicsRequirements);
  Utils::checkXrResult(xr_result, "Failed to retrieve Vulkan graphics requirements");

  // and finally retrieve the created graphics queue
  vkGetDeviceQueue(m_device, m_queue_family_index, 0, &m_graphics_queue);
  // vkGetDeviceQueue(m_device, m_queue_family_index, 0, &m_present_queue);

  //------------------------------------------------------------------------------------------------------
  // Render pass
  //------------------------------------------------------------------------------------------------------
  // We only have a single color buffer attachment (represented by one of the images from the swapchain)
  VkAttachmentDescription color_attachment{};
  color_attachment.format = s_color_format;
  color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
  color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  color_attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  color_attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // Setup a reference to the color attachment we created beforehand
  VkAttachmentReference color_attachment_reference{};
  color_attachment_reference.attachment = 0;
  color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  // And we have a single depth buffer attachment
  VkAttachmentDescription depth_attachment{};
  depth_attachment.format = VK_FORMAT_D32_SFLOAT;
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
  VkSubpassDescription subpass_description{};
  subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass_description.colorAttachmentCount = 1;
  subpass_description.pColorAttachments = &color_attachment_reference;
  subpass_description.pDepthStencilAttachment = &depth_attachment_reference;

  // Create render subpass dependency
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
  renderPassInfo.pSubpasses = &subpass_description;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  result = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_render_pass);
  Utils::checkVkResult(result, "Failed to create render pass");
}

void VulkanHandler::setupRenderer() {
  VkResult result;

  // Round up to minUniformBufferOffsetAlignment
  VkDeviceSize model_uniform_buffer_object_size = sizeof(ModelUniformBufferObject);
  VkPhysicalDeviceProperties properties{};
  vkGetPhysicalDeviceProperties(m_physical_device, &properties);
  VkDeviceSize min_alignment = properties.limits.minUniformBufferOffsetAlignment;
  m_aligned_size = (model_uniform_buffer_object_size + min_alignment - 1) & ~(min_alignment - 1);

  //------------------------------------------------------------------------------------------------------
  // Uniform buffer
  //------------------------------------------------------------------------------------------------------
  // Create uniform buffer
  m_uniform_buffer = new Buffer(
    m_device,
    m_physical_device,
    m_aligned_size * s_max_models_in_scene,
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
  );

  // Create global uniform buffer
  m_global_uniform_buffer = new Buffer(
    m_device,
    m_physical_device,
    sizeof(GlobalUniformBufferObject),
    VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
  );

  //------------------------------------------------------------------------------------------------------
  // Descriptor set layouts
  //------------------------------------------------------------------------------------------------------
  // Global UBO layout (set = 0)
  VkDescriptorSetLayoutBinding global_ubo_layout_binding{};
  global_ubo_layout_binding.binding = 0;
  global_ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  global_ubo_layout_binding.descriptorCount = 1;
  global_ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutCreateInfo global_layout_create_info{};
  global_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  global_layout_create_info.bindingCount = 1;
  global_layout_create_info.pBindings = &global_ubo_layout_binding;

  result = vkCreateDescriptorSetLayout(m_device, &global_layout_create_info, nullptr, &m_global_descriptor_set_layout);
  Utils::checkVkResult(result, "Failed to create global descriptor set layout");
  
  // Model uniform buffer object (set = 1)
  VkDescriptorSetLayoutBinding ubo_layout_binding{};
  ubo_layout_binding.binding = 0;
  ubo_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  ubo_layout_binding.descriptorCount = 1;
  ubo_layout_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  // Combined image sampler
  // VkDescriptorSetLayoutBinding sampler_layout_binding{};
  // sampler_layout_binding.binding = 1;
  // sampler_layout_binding.descriptorCount = 1;
  // sampler_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  // sampler_layout_binding.pImmutableSamplers = nullptr;
  // sampler_layout_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  // std::array<VkDescriptorSetLayoutBinding, 2> bindings = { ubo_layout_binding, sampler_layout_binding };
  VkDescriptorSetLayoutCreateInfo layout_create_info{};
  layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  layout_create_info.bindingCount = 1;
  layout_create_info.pBindings = &ubo_layout_binding;

  result = vkCreateDescriptorSetLayout(m_device, &layout_create_info, nullptr, &m_descriptor_set_layout);
  Utils::checkVkResult(result, "Failed to create the descriptor set layout");

  //------------------------------------------------------------------------------------------------------
  // Descriptor pool
  //------------------------------------------------------------------------------------------------------
  // Create global descriptor pool
  VkDescriptorPoolSize global_descriptor_pool_size{};
  global_descriptor_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  global_descriptor_pool_size.descriptorCount = 1;

  VkDescriptorPoolCreateInfo global_descriptor_pool_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
  global_descriptor_pool_create_info.poolSizeCount = 1;
  global_descriptor_pool_create_info.pPoolSizes = &global_descriptor_pool_size;
  global_descriptor_pool_create_info.maxSets = 1;

  VkDescriptorPool global_descriptor_pool;
  result = vkCreateDescriptorPool(m_device, &global_descriptor_pool_create_info, nullptr, &global_descriptor_pool);
  Utils::checkVkResult(result, "Failed to create global descriptor pool");

  // Create local descriptor pool
  VkDescriptorPoolSize descriptor_pool_size;
  descriptor_pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  descriptor_pool_size.descriptorCount = 1u;

  VkDescriptorPoolCreateInfo descriptor_pool_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
  descriptor_pool_create_info.poolSizeCount = 1u;
  descriptor_pool_create_info.pPoolSizes = &descriptor_pool_size;
  descriptor_pool_create_info.maxSets = 1u;

  VkDescriptorPool descriptor_pool;
  result = vkCreateDescriptorPool(m_device, &descriptor_pool_create_info, nullptr, &descriptor_pool);
  Utils::checkVkResult(result, "Failed to create descriptor pool");

  //------------------------------------------------------------------------------------------------------
  // Descriptor set
  //------------------------------------------------------------------------------------------------------
  // Allocate global descriptor set
  VkDescriptorSetAllocateInfo global_descriptor_set_allocate_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
  global_descriptor_set_allocate_info.descriptorPool = global_descriptor_pool;
  global_descriptor_set_allocate_info.descriptorSetCount = 1;
  global_descriptor_set_allocate_info.pSetLayouts = &m_global_descriptor_set_layout;

  result = vkAllocateDescriptorSets(m_device, &global_descriptor_set_allocate_info, &m_global_descriptor_set);
  Utils::checkVkResult(result, "Failed to allocate global descriptor set from pool");

  VkDescriptorBufferInfo global_descriptor_buffer_info{};
  global_descriptor_buffer_info.buffer = m_global_uniform_buffer->getBuffer();
  global_descriptor_buffer_info.offset = 0;
  global_descriptor_buffer_info.range = VK_WHOLE_SIZE;

  VkWriteDescriptorSet global_write_descriptor_set{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
  global_write_descriptor_set.dstSet = m_global_descriptor_set;
  global_write_descriptor_set.pBufferInfo = &global_descriptor_buffer_info;
  global_write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  global_write_descriptor_set.descriptorCount = 1;
  global_write_descriptor_set.dstBinding = 0;
  global_write_descriptor_set.dstArrayElement = 0;

  vkUpdateDescriptorSets(m_device, 1, &global_write_descriptor_set, 0, nullptr);

  // Allocate local descriptor set
  VkDescriptorSetAllocateInfo descriptor_set_allocate_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
  descriptor_set_allocate_info.descriptorPool = descriptor_pool;
  descriptor_set_allocate_info.descriptorSetCount = 1u;
  descriptor_set_allocate_info.pSetLayouts = &m_descriptor_set_layout;

  result = vkAllocateDescriptorSets(m_device, &descriptor_set_allocate_info, &m_descriptor_set);
  Utils::checkVkResult(result, "Failed to allocate descriptor set from pool");

  VkDescriptorBufferInfo descriptor_buffer_info;
  descriptor_buffer_info.buffer = m_uniform_buffer->getBuffer();
  descriptor_buffer_info.offset = 0u;
  descriptor_buffer_info.range = VK_WHOLE_SIZE;

  VkWriteDescriptorSet write_descriptor_set{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
  write_descriptor_set.dstSet = m_descriptor_set;
  write_descriptor_set.pBufferInfo = &descriptor_buffer_info;
  write_descriptor_set.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
  write_descriptor_set.descriptorCount = 1u;
  write_descriptor_set.dstBinding = 0u;
  write_descriptor_set.dstArrayElement = 0u;

  vkUpdateDescriptorSets(m_device, 1u, &write_descriptor_set, 0u, nullptr);

  //------------------------------------------------------------------------------------------------------
  // Pipeline layout
  //------------------------------------------------------------------------------------------------------
  std::array<VkDescriptorSetLayout, 2> set_layouts = {
    m_global_descriptor_set_layout,  // set = 0
    m_descriptor_set_layout          // set = 1 (local UBO)
  };

  VkPipelineLayoutCreateInfo pipeline_layout_info{};
  pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size());
  pipeline_layout_info.pSetLayouts = set_layouts.data();

  result = vkCreatePipelineLayout(m_device, &pipeline_layout_info, nullptr, &m_pipeline_layout);
  Utils::checkVkResult(result, "Failed to create pipeline layout");

  //------------------------------------------------------------------------------------------------------
  // Shader modules
  //------------------------------------------------------------------------------------------------------
  // Load shader bytecodes
  static std::vector<char> vertex_shader_code = Utils::readFile(SHADERS_FOLDER "vk/basic.vert.spv");
  static std::vector<char> fragment_shader_code = Utils::readFile(SHADERS_FOLDER "vk/basic.frag.spv");

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

  std::vector<VkPipelineShaderStageCreateInfo> shader_stages = { vertex_shader_stage_create_info, fragment_shader_stage_create_info };

  //------------------------------------------------------------------------------------------------------
  // Dynamic state
  //------------------------------------------------------------------------------------------------------
  const std::array dynamic_states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

  VkPipelineDynamicStateCreateInfo dynamic_state_create_info{};
  dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
  dynamic_state_create_info.pDynamicStates = dynamic_states.data();

  //------------------------------------------------------------------------------------------------------
  // Vertex input
  //------------------------------------------------------------------------------------------------------
  auto vertex_binding_description = Vertex::getBindingDescription();
  auto vertex_attribute_descriptions = Vertex::getAttributeDescriptions();
  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertex_input_info.vertexBindingDescriptionCount = 1;
  vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertex_attribute_descriptions.size());
  vertex_input_info.pVertexBindingDescriptions = &vertex_binding_description;
  vertex_input_info.pVertexAttributeDescriptions = vertex_attribute_descriptions.data();

  //------------------------------------------------------------------------------------------------------
  // Input assembly
  //------------------------------------------------------------------------------------------------------
  // Describe the input topology and if primitive restart is enabled
  VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
  input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  input_assembly_info.primitiveRestartEnable = VK_FALSE;

  //------------------------------------------------------------------------------------------------------
  // Viewports and scissors
  //------------------------------------------------------------------------------------------------------
  // Setup the viewport state
  VkPipelineViewportStateCreateInfo viewport_state_info{};
  viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_state_info.viewportCount = 1;
  viewport_state_info.scissorCount = 1;

  //------------------------------------------------------------------------------------------------------
  // Rasterizer
  //------------------------------------------------------------------------------------------------------
  // Setup the rasterizer
  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  //------------------------------------------------------------------------------------------------------
  // Multisampling
  //------------------------------------------------------------------------------------------------------
  // Keep multisampling disabled for now
  VkPipelineMultisampleStateCreateInfo multisampling_info{};
  multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_info.sampleShadingEnable = VK_FALSE;
  multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  //------------------------------------------------------------------------------------------------------
  // Depth buffer and stencil
  //------------------------------------------------------------------------------------------------------
  VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
  depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depth_stencil_info.depthTestEnable = VK_TRUE;
  depth_stencil_info.depthWriteEnable = VK_TRUE;
  depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
  depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
  depth_stencil_info.stencilTestEnable = VK_FALSE;

  //------------------------------------------------------------------------------------------------------
  // Color blending
  //------------------------------------------------------------------------------------------------------
  VkPipelineColorBlendAttachmentState color_blend_attachment_state{};
  color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment_state.blendEnable = VK_TRUE;
  color_blend_attachment_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
  color_blend_attachment_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  color_blend_attachment_state.colorBlendOp = VK_BLEND_OP_ADD;
  color_blend_attachment_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  color_blend_attachment_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  color_blend_attachment_state.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineColorBlendStateCreateInfo color_blend_info{};
  color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_info.logicOpEnable = VK_FALSE;
  color_blend_info.attachmentCount = 1;
  color_blend_info.pAttachments = &color_blend_attachment_state;

  //------------------------------------------------------------------------------------------------------
  // Graphics pipeline
  //------------------------------------------------------------------------------------------------------
  // Setup the pipeline create info
  VkGraphicsPipelineCreateInfo pipeline_create_info{};
  pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_create_info.layout = m_pipeline_layout;
  pipeline_create_info.stageCount = static_cast<uint32_t>(shader_stages.size());
  pipeline_create_info.pStages = shader_stages.data();;
  pipeline_create_info.pVertexInputState = &vertex_input_info;
  pipeline_create_info.pInputAssemblyState = &input_assembly_info;
  pipeline_create_info.pViewportState = &viewport_state_info;
  pipeline_create_info.pRasterizationState = &rasterizer;
  pipeline_create_info.pMultisampleState = &multisampling_info;
  pipeline_create_info.pDepthStencilState = nullptr; // Optional
  pipeline_create_info.pColorBlendState = &color_blend_info;
  pipeline_create_info.pDynamicState = &dynamic_state_create_info;
  pipeline_create_info.pDepthStencilState = &depth_stencil_info;
  pipeline_create_info.renderPass = m_render_pass;
  pipeline_create_info.subpass = 0;

  result = vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &m_graphics_pipeline);
  Utils::checkVkResult(result, "Failed to create the graphics pipeline");

  // Make sure to cleanup the shader modules
  vkDestroyShaderModule(m_device, fragment_shader_module, nullptr);
  vkDestroyShaderModule(m_device, vertex_shader_module, nullptr);

  //------------------------------------------------------------------------------------------------------
  // Command pool
  //------------------------------------------------------------------------------------------------------
  VkCommandPoolCreateInfo pool_create_info{};
  pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
  pool_create_info.queueFamilyIndex = m_queue_family_index;

  result = vkCreateCommandPool(m_device, &pool_create_info, nullptr, &m_command_pool);
  Utils::checkVkResult(result, "Failed to create the command pool");

  //------------------------------------------------------------------------------------------------------
  // Command buffer
  //------------------------------------------------------------------------------------------------------
  VkCommandBufferAllocateInfo buffer_allocate_info{};
  buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  buffer_allocate_info.commandPool = m_command_pool;
  buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  buffer_allocate_info.commandBufferCount = 1u;

  result = vkAllocateCommandBuffers(m_device, &buffer_allocate_info, &m_command_buffer);
  Utils::checkVkResult(result, "Failed to allocate command buffers");

  //------------------------------------------------------------------------------------------------------
  // Sync objects
  //------------------------------------------------------------------------------------------------------
  // Create memory fence
  VkFenceCreateInfo fence_create_info{};
  fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  // Create the fence in the signalled state such that the first `vkWaitForFences` call
  // does not block indefinitely.
  fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
  result = vkCreateFence(m_device, &fence_create_info, nullptr, &m_fence);
  Utils::checkVkResult(result, "Failed to create fence");
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

void VulkanHandler::renderFrame(glm::mat4 view, glm::mat4 projection, VkFramebuffer framebuf,
                                VkExtent2D resolution, std::function<void(RenderContext&)> draw_callback) {
  VkResult result;

  //------------------------------------------------------------------------------------------------------
  // Wait for the previous frame
  //------------------------------------------------------------------------------------------------------
  // Wait for the previous frame to be finished (by waiting for the fence
  // to be signalled)
  result = vkWaitForFences(m_device, 1u, &m_fence, VK_TRUE, UINT64_MAX);
  Utils::checkVkResult(result, "Failed to wait for memory fence");

  //------------------------------------------------------------------------------------------------------
  // Reset sync objects
  //------------------------------------------------------------------------------------------------------
  // Reset the fence to the unsignalled state
  result = vkResetFences(m_device, 1u, &m_fence);
  Utils::checkVkResult(result, "Failed to reset fence to unsignalled state!");

  // Reset the command buffer such that we can record into it
  result = vkResetCommandBuffer(m_command_buffer,  0);
  Utils::checkVkResult(result, "failed to reset command buffer!");

  //------------------------------------------------------------------------------------------------------
  // Begin recording the command buffer
  //------------------------------------------------------------------------------------------------------
  VkCommandBufferBeginInfo begin_info{};
  begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  result = vkBeginCommandBuffer(m_command_buffer, &begin_info);
  Utils::checkVkResult(result, "failed to begin recording command buffer!");

  //------------------------------------------------------------------------------------------------------
  // Setup render pass
  //------------------------------------------------------------------------------------------------------
  // Values to use to clear the buffers (color, set to black, depth, set to 1);
  std::array<VkClearValue, 2> clear_values{};
  clear_values[0].color = {{0.5f, 0.5f, 0.5f, 1.0f}};
  clear_values[1].depthStencil = {1.0f, 0};

  // Start the render pass
  VkRenderPassBeginInfo render_pass_info{};
  render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  render_pass_info.renderPass = m_render_pass;
  render_pass_info.framebuffer = framebuf;
  render_pass_info.renderArea.offset = { 0, 0 };
  render_pass_info.renderArea.extent = resolution;
  render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
  render_pass_info.pClearValues = clear_values.data();
  vkCmdBeginRenderPass(m_command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

  //------------------------------------------------------------------------------------------------------
  // Bind the graphics pipeline
  //------------------------------------------------------------------------------------------------------
  vkCmdBindPipeline(m_command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphics_pipeline);

  //------------------------------------------------------------------------------------------------------
  // Set viewport
  //------------------------------------------------------------------------------------------------------
  VkViewport viewport;
  viewport.x = static_cast<float>(render_pass_info.renderArea.offset.x);
  viewport.y = static_cast<float>(render_pass_info.renderArea.offset.y);
  viewport.width = static_cast<float>(render_pass_info.renderArea.extent.width);
  viewport.height = static_cast<float>(render_pass_info.renderArea.extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(m_command_buffer, 0u, 1u, &viewport);

  //------------------------------------------------------------------------------------------------------
  // Set scissor
  //------------------------------------------------------------------------------------------------------
  VkRect2D scissor;
  scissor.offset = render_pass_info.renderArea.offset;
  scissor.extent = render_pass_info.renderArea.extent;
  vkCmdSetScissor(m_command_buffer, 0u, 1u, &scissor);
  
  //------------------------------------------------------------------------------------------------------
  // Draw the scene
  //------------------------------------------------------------------------------------------------------
  // Prepare the render context
  RenderContext ctx{};
  ctx.command_buffer = m_command_buffer;
  ctx.model_uniform_buffer = m_uniform_buffer;
  ctx.pipeline_layout = m_pipeline_layout;
  ctx.descriptor_set = m_descriptor_set;
  ctx.aligned_size = m_aligned_size;

  // Update global buffer
  GlobalUniformBufferObject global_uniform_buffer_object{};
  global_uniform_buffer_object.view_projection = projection * view;
  global_uniform_buffer_object.light_vector = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
  global_uniform_buffer_object.light_color = glm::vec3(0.5f, 0.5f, 0.5f);
  global_uniform_buffer_object.ambient_color = glm::vec3(0.2f, 0.2f, 0.2f);

  m_global_uniform_buffer->loadData(global_uniform_buffer_object);

  // Bind global descriptor set (camera)
  vkCmdBindDescriptorSets(
    m_command_buffer,
    VK_PIPELINE_BIND_POINT_GRAPHICS,
    m_pipeline_layout,
    0, 1, &m_global_descriptor_set, // set = 0
    0, nullptr
  );

  draw_callback(ctx);

  //------------------------------------------------------------------------------------------------------
  // End the render pass
  //------------------------------------------------------------------------------------------------------
  vkCmdEndRenderPass(m_command_buffer);

  //------------------------------------------------------------------------------------------------------
  // End recording the command buffer
  //------------------------------------------------------------------------------------------------------
  result = vkEndCommandBuffer(m_command_buffer);
  Utils::checkVkResult(result, "failed to record command buffer!");

  //------------------------------------------------------------------------------------------------------
  // Submit the command buffer
  //------------------------------------------------------------------------------------------------------
  VkSubmitInfo submit_info{};
  submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submit_info.commandBufferCount = 1u;
  submit_info.pCommandBuffers = &m_command_buffer;

  //------------------------------------------------------------------------------------------------------
  // Submit the queue
  //------------------------------------------------------------------------------------------------------
  // Actually submit the queue, which will also signal the `inFlightFence` on successful
  // completion.
  result = vkQueueSubmit(m_graphics_queue, 1, &submit_info, m_fence);
  Utils::checkVkResult(result, "failed to submit draw command buffer!");
}

VkInstance VulkanHandler::getInstance() {
  return m_vk_instance;
}

VkPhysicalDevice VulkanHandler::getPhysicalDevice() {
  return m_physical_device;
}

VkDevice VulkanHandler::getLogicalDevice() {
  return m_device;
}

uint32_t VulkanHandler::getQueueFamilyIndex() {
  return m_queue_family_index;
}

VkRenderPass VulkanHandler::getRenderPass() {
  return m_render_pass;
}
