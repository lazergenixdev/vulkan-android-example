#include <engine.hpp>
#include <cstdio>

int on_create() {
    LOGI("????");
    return 1;
}

// Most of this graphics stuff should not be here,
//   but I am too lazy to refactor, so it shall
//   stay here ._.

void check_layers()
{
    uint32_t layer_count {};
    vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

    std::vector<VkLayerProperties> available(layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available.data());

    LOGIf("Number of layers available: %d", layer_count);

    for (auto layer: available) {
        LOGIf(" > Layer: \"%s\" (%s)", layer.layerName, layer.description);
    }

    uint32_t extension_count;
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);

    std::vector<VkExtensionProperties> extensions(extension_count);
    vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data());

    LOGIf("Number of extensions available: %d", extension_count);

    for (auto ext: extensions) {
        LOGIf(" > Extension: \"%s\"", ext.extensionName);
    }
}

VkInstance create_instance() {
    VkInstance instance {};

    check_layers();
    char const* layer_names[] {
        "VK_LAYER_KHRONOS_validation",
    };

    char const* extension_names [] {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if   OS_WINDOWS
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#elif OS_ANDROID
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#endif
    };

    VkInstanceCreateInfo instance_info {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .enabledLayerCount = std::size(layer_names),
        .ppEnabledLayerNames = layer_names,
        .enabledExtensionCount = std::size(extension_names),
        .ppEnabledExtensionNames = extension_names,
    };

    VkResult result = vkCreateInstance(&instance_info, nullptr, &instance);

    LOGIf("vkCreateInstance(..) => %d [instance=%p]", result, instance);

    return instance;
}

VkSurfaceKHR create_surface() {
    VkSurfaceKHR surface {};

#if   OS_WINDOWS
    VkWin32SurfaceCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
        .hwnd = engine.window,
    };
    VkResult result = vkCreateWin32SurfaceKHR(gfx.instance, &createInfo, nullptr, &surface);
#elif OS_ANDROID
    VkAndroidSurfaceCreateInfoKHR createInfo = {
        .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
        .window = engine.window,
    };
    VkResult result = vkCreateAndroidSurfaceKHR(gfx.instance, &createInfo, nullptr, &surface);
#endif

    LOGIf("vkCreateAndroidSurfaceKHR(..) => %d [surface=%p]", result, surface);

    return surface;
}

VkPhysicalDevice pick_physical_device() {
    uint32_t physical_device_count {};
    vkEnumeratePhysicalDevices(gfx.instance, &physical_device_count, nullptr);
    std::vector<VkPhysicalDevice> physical_devices { physical_device_count };
    vkEnumeratePhysicalDevices(gfx.instance, &physical_device_count, physical_devices.data());

    LOGIf("Number of physical devices: %d", physical_device_count);

    VkPhysicalDeviceProperties properties {};
    vkGetPhysicalDeviceProperties(physical_devices[0], &properties);
    LOGIf("Physical device: \"%s\" (vendor=%d, device=%d)", properties.deviceName, properties.vendorID, properties.deviceID);

    return physical_devices[0];
}

VkDevice create_device() {
    uint32_t queue_family_count {};
    vkGetPhysicalDeviceQueueFamilyProperties(gfx.physical_device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families { queue_family_count };
    vkGetPhysicalDeviceQueueFamilyProperties(gfx.physical_device, &queue_family_count, queue_families.data());

    LOGIf("Number of queue families: %d", queue_family_count);

    int i = 0;
    for (auto const& queue_family: queue_families) {
        LOGIf(" > Queue %d: count=%d flags=%X", i++, queue_family.queueCount, queue_family.queueFlags);
    }

    VkDevice device {};

    float queuePriority = 1.0f;

    VkDeviceQueueCreateInfo queue_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .queueFamilyIndex = 0,  // Simplified assumption
        .queueCount = 1,
        .pQueuePriorities = &queuePriority,
    };

    char const* extension_names [] {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    VkDeviceCreateInfo device_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_info,
        .enabledExtensionCount = std::size(extension_names),
        .ppEnabledExtensionNames = extension_names,
    };

    VkResult result = vkCreateDevice(gfx.physical_device, &device_info, nullptr, &device);

    LOGIf("vkCreateDevice(..) => %d [device=%p]", result, device);

    vkGetDeviceQueue(device, 0, 0, &gfx.queue);

    LOGIf("vkGetDeviceQueue(..) [queue=%p]", result, gfx.queue);

    return device;
}

VkSwapchainKHR create_swap_chain(uint32_t width, uint32_t height) {
    VkSwapchainKHR swap_chain {};

    VkSurfaceCapabilitiesKHR capabilities {};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gfx.physical_device, gfx.surface, &capabilities);

    LOGIf(" >     Current: width=%d height=%d", capabilities.currentExtent.width,  capabilities.currentExtent.height);
    LOGIf(" >       Width: min=%d max=%d", capabilities.minImageExtent.width,  capabilities.maxImageExtent.width);
    LOGIf(" >      Height: min=%d max=%d", capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    LOGIf(" > Image count: min=%d max=%d", capabilities.minImageCount,  capabilities.maxImageCount);
    LOGIf(" >   Composite: flags=%d", capabilities.supportedCompositeAlpha);

    uint32_t format_count {};
    vkGetPhysicalDeviceSurfaceFormatsKHR(gfx.physical_device, gfx.surface, &format_count, nullptr);

    std::vector<VkSurfaceFormatKHR> formats { format_count };
    vkGetPhysicalDeviceSurfaceFormatsKHR(gfx.physical_device, gfx.surface, &format_count, formats.data());

    gfx.swap_format = formats[0].format;
    LOGIf(" >      Format: %d", gfx.swap_format);

    if (width && height) {
        gfx.swap_extent = {.width = width, .height = height};
    }
    else {
        gfx.swap_extent = capabilities.currentExtent;
    }

    VkSwapchainCreateInfoKHR swap_chain_info {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .surface = gfx.surface,
        .minImageCount = capabilities.minImageCount,
        .imageFormat = gfx.swap_format,
        .imageColorSpace = formats[0].colorSpace,
        .imageExtent = gfx.swap_extent,
        .imageArrayLayers = 1, /* For non-stereoscopic-3D applications, this value is 1 */
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
        .presentMode = VK_PRESENT_MODE_FIFO_KHR,
        .clipped = VK_TRUE,
    };

    VkResult result = vkCreateSwapchainKHR(gfx.device, &swap_chain_info, nullptr, &swap_chain);

    LOGIf("vkCreateSwapchainKHR(..) => %d [swap_chain=%p]", result, swap_chain);

    uint32_t image_count {};
    vkGetSwapchainImagesKHR(gfx.device, swap_chain, &image_count, nullptr);

    LOGIf(" > Image count = %d", image_count);

    gfx.swap_chain_images.resize(image_count);
    vkGetSwapchainImagesKHR(gfx.device, swap_chain, &image_count, gfx.swap_chain_images.data());

    return swap_chain;
}

void create_image_views() {
    gfx.swap_chain_views.resize(gfx.swap_chain_images.size());

    int i = 0;
    for (auto image: gfx.swap_chain_images) {
        VkImageViewCreateInfo createInfo {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = gfx.swap_format,
            .components = {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };

        vkCreateImageView(gfx.device, &createInfo, nullptr, &gfx.swap_chain_views[i++]);
    }
}

VkRenderPass create_render_pass() {
    VkRenderPass render_pass {};

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = gfx.swap_format;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult result = vkCreateRenderPass(gfx.device, &renderPassInfo, nullptr, &render_pass);

    LOGIf("vkCreateRenderPass(..) => %d [render_pass=%p]", result, render_pass);

    return render_pass;
}

struct vert_shader {
    #include "shader.vert.inl"
};
struct frag_shader {
    #include "shader.frag.inl"
};

VkPipeline create_pipeline() {
    VkPipeline pipeline {};

    VkShaderModule vert_shader_module {}, frag_shader_module {};
    VkShaderModuleCreateInfo shader_info {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
    };

    shader_info.codeSize = vert_shader::size;
    shader_info.pCode    = vert_shader::data;
    vkCreateShaderModule(gfx.device, &shader_info, nullptr, &vert_shader_module);

    shader_info.codeSize = frag_shader::size;
    shader_info.pCode    = frag_shader::data;
    vkCreateShaderModule(gfx.device, &shader_info, nullptr, &frag_shader_module);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vert_shader_module;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = frag_shader_module;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPushConstantRange push {
        .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
        .size = sizeof(float),
    };
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = &push;
    pipelineLayoutInfo.pushConstantRangeCount = 1;

    VkResult result = vkCreatePipelineLayout(gfx.device, &pipelineLayoutInfo, nullptr, &gfx.pipeline_layout);

    LOGIf("vkCreatePipelineLayout(..) => %d [pipeline_layout=%p]", result, gfx.pipeline_layout);

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = gfx.pipeline_layout;
    pipelineInfo.renderPass = gfx.render_pass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    vkCreateGraphicsPipelines(gfx.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline);

    LOGIf("vkCreateGraphicsPipelines(..) => %d [pipeline=%p]", result, pipeline);

    vkDestroyShaderModule(gfx.device, frag_shader_module, nullptr);
    vkDestroyShaderModule(gfx.device, vert_shader_module, nullptr);

    return pipeline;
}

void create_frame_buffers() {
    gfx.swap_chain_frame_buffers.resize(gfx.swap_chain_views.size());

    for (size_t i = 0; i < gfx.swap_chain_views.size(); i++) {
        VkImageView attachments[] = { gfx.swap_chain_views[i] };

        VkFramebufferCreateInfo framebufferInfo {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = gfx.render_pass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = gfx.swap_extent.width;
        framebufferInfo.height = gfx.swap_extent.height;
        framebufferInfo.layers = 1;

        vkCreateFramebuffer(gfx.device, &framebufferInfo, nullptr, &gfx.swap_chain_frame_buffers[i]);
    }
}

VkCommandPool create_command_pool() {
    VkCommandPool command_pool {};

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = 0;

    VkResult result = vkCreateCommandPool(gfx.device, &poolInfo, nullptr, &command_pool);

    LOGIf("vkCreateCommandPool(..) => %d [command_pool=%p]", result, command_pool);

    return command_pool;
}

VkCommandBuffer create_command_buffer() {
    VkCommandBuffer command_buffer {};

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = gfx.command_pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(gfx.device, &allocInfo, &command_buffer);

    LOGIf("vkAllocateCommandBuffers(..) => %d [command_buffer=%p]", result, command_buffer);

    return command_buffer;
}

void create_sync_primitives() {
    VkSemaphoreCreateInfo semaphoreInfo {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    int result {};

    result |= vkCreateSemaphore(gfx.device, &semaphoreInfo, nullptr, &gfx.write_semaphore);
    result |= vkCreateSemaphore(gfx.device, &semaphoreInfo, nullptr, &gfx.read_semaphore);
    result |= vkCreateFence(gfx.device, &fenceInfo, nullptr, &gfx.fence);

    LOGIf("Create Fence + Semaphores => %d", result);
}

void update_swap_chain(uint32_t width, uint32_t height) {
    LOGI("creating new swap chain!!");
    os_lock_mutex(engine.render_lock);
    vkDeviceWaitIdle(gfx.device);
    for (auto view: gfx.swap_chain_views) {
        vkDestroyImageView(gfx.device, view, nullptr);
    }
    for (auto frame_buffer: gfx.swap_chain_frame_buffers) {
        vkDestroyFramebuffer(gfx.device, frame_buffer, nullptr);
    }
    vkDestroySwapchainKHR(gfx.device, gfx.swap_chain, nullptr);
    gfx.swap_chain = create_swap_chain(width, height);
    create_image_views();
    create_frame_buffers();
    os_unlock_mutex(engine.render_lock);
}

void draw_frame() {
    static float t = 0.0f;

    vkWaitForFences(gfx.device, 1, &gfx.fence, VK_TRUE, UINT64_MAX);
    vkResetFences(gfx.device, 1, &gfx.fence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(gfx.device, gfx.swap_chain, UINT64_MAX, gfx.write_semaphore, VK_NULL_HANDLE, &imageIndex);

    vkResetCommandBuffer(gfx.command_buffer, 0);

    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        vkBeginCommandBuffer(gfx.command_buffer, &beginInfo);

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = gfx.render_pass;
        renderPassInfo.framebuffer = gfx.swap_chain_frame_buffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = gfx.swap_extent;

        VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(gfx.command_buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(gfx.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx.pipeline);

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(gfx.swap_extent.width);
        viewport.height = static_cast<float>(gfx.swap_extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(gfx.command_buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = gfx.swap_extent;
        vkCmdSetScissor(gfx.command_buffer, 0, 1, &scissor);

        float push = t;
        vkCmdPushConstants(gfx.command_buffer, gfx.pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, 4, &push);

        vkCmdDraw(gfx.command_buffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(gfx.command_buffer);

        vkEndCommandBuffer(gfx.command_buffer);
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { gfx.write_semaphore };
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &gfx.command_buffer;

    VkSemaphore signalSemaphores[] = { gfx.read_semaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkQueueSubmit(gfx.queue, 1, &submitInfo, gfx.fence);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = { gfx.swap_chain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(gfx.queue, &presentInfo);

    t += 0.01f;
}

void main_loop()
{
    char message[256];
    snprintf(message, sizeof(message), "Called from %s!  :)", __PRETTY_FUNCTION__);
    os::show_dialog("C++ YAY!",
                    " VUID-vkDestroyDevice-device-05137(ERROR / SPEC): msgNum: 1215490720 - Validation Error: [ VUID-vkDestroyDevice-device-05137 ] Object 0: handle = 0x7db7375fc0, type = VK_OBJECT_TYPE_COMMAND_BUFFER; | MessageID = 0x4872eaa0 | vkDestroyDevice():  OBJ ERROR : For VkDevice 0x7db7604340[], VkCommandBuffer 0x7db7375fc0[] has not been destroyed. The Vulkan spec states: All child objects created on device must have been destroyed prior to destroying device (https://www.khronos.org/registry/vulkan/specs/1.3-extensions/html/vkspec.html#VUID-vkDestroyDevice-device-05137)\n"
                    "                                                                                                        Objects: 1\n"
                    "                                                                                                            [0] 0x7db7375fc0, type: 6, name: NULL"
                    );

    while (!engine.want_exit) {
        os_lock_mutex(engine.render_lock);
        draw_frame();
        os_unlock_mutex(engine.render_lock);
    }
}

void Graphics::create() {
    instance = create_instance();
    surface = create_surface();
    physical_device = pick_physical_device();
    device = create_device();
    swap_chain = create_swap_chain(0, 0);
    create_image_views();
    render_pass = create_render_pass();
    pipeline = create_pipeline();
    create_frame_buffers();
    command_pool = create_command_pool();
    command_buffer = create_command_buffer();
    create_sync_primitives();
    LOGI("Graphics Created!!!");
}

void Graphics::destroy() {
    if (device) vkDeviceWaitIdle(device);
    if (fence) vkDestroyFence(device, fence, nullptr);
    if (read_semaphore) vkDestroySemaphore(device, read_semaphore, nullptr);
    if (write_semaphore) vkDestroySemaphore(device, write_semaphore, nullptr);
    if (command_pool) vkDestroyCommandPool(device, command_pool, nullptr);
    if (render_pass) vkDestroyRenderPass(device, render_pass, nullptr);
    if (pipeline_layout) vkDestroyPipelineLayout(device, pipeline_layout, nullptr);
    if (pipeline) vkDestroyPipeline(device, pipeline, nullptr);
    for (auto frame_buffer: swap_chain_frame_buffers) vkDestroyFramebuffer(device, frame_buffer, nullptr);
    for (auto view: swap_chain_views) vkDestroyImageView(device, view, nullptr);
    if (swap_chain) vkDestroySwapchainKHR(device, swap_chain, nullptr);
    if (device) vkDestroyDevice(device, nullptr);
    if (surface) vkDestroySurfaceKHR(instance, surface, nullptr);
    if (instance) vkDestroyInstance(instance, nullptr);
}
