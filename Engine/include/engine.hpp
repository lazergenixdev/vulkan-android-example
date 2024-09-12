#pragma once
#include <platform.hpp>
#include <vulkan/vulkan.h>
#include <vector>

struct Engine {
    bool want_exit = false;
    os::Mutex  render_lock {};
    os::Thread main_thread {};
    os::Window window {};

    auto create  (os::Instance const&, int) -> int;
    auto run     () -> void;
    auto destroy () -> int;
};

struct Graphics {
    VkFormat   swap_format {};
    VkExtent2D swap_extent {};

    VkInstance                  instance         {};
    VkSurfaceKHR                surface          {};
    VkPhysicalDevice            physical_device  {};
    VkDevice                    device           {};
    VkQueue                     queue            {};
    VkSwapchainKHR              swap_chain       {};
    VkRenderPass                render_pass      {};
    VkPipelineLayout            pipeline_layout  {};
    VkPipeline                  pipeline         {};
    VkCommandPool               command_pool     {};
    VkCommandBuffer             command_buffer   {};
    VkSemaphore                 write_semaphore  {};
    VkSemaphore                 read_semaphore   {};
    VkFence                     fence            {};

    std::vector<VkImage>        swap_chain_images {};
    std::vector<VkImageView>    swap_chain_views {};
    std::vector<VkFramebuffer>  swap_chain_frame_buffers {};

    void create();
    void destroy();
};

extern Engine engine;
extern Graphics gfx;

extern int on_create();
