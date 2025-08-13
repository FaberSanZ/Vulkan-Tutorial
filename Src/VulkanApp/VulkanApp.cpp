
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#include <iostream>
#include <vector>
#include <windows.h>
#include "vulkan/vulkan.h"
#include <vulkan/vulkan_win32.h>
#include <glfw3.h>
#include <glfw3native.h>

int main()
{
    uint32_t width = 1200;
    uint32_t height = 820;

    // 0. GLFW Window
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
    HWND hwnd = glfwGetWin32Window(window); // Get the Win32 HWND from GLFW

	// Initialize Vulkan
    // 1. Instance
    VkApplicationInfo appInfo { VK_STRUCTURE_TYPE_APPLICATION_INFO };
	appInfo.apiVersion = VK_API_VERSION_1_3; // Use Vulkan 1.3

    const char* extensions[] = { "VK_KHR_surface", "VK_KHR_win32_surface" };

    VkInstanceCreateInfo instanceInfo { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledExtensionCount = 2;
    instanceInfo.ppEnabledExtensionNames = extensions;

    VkInstance instance;
    vkCreateInstance(&instanceInfo, nullptr, &instance);


	// 3. Get GPU 
    uint32_t gpuCount;
    std::vector<VkPhysicalDevice> gpus(gpuCount);
    vkEnumeratePhysicalDevices(instance, &gpuCount, nullptr);
    vkEnumeratePhysicalDevices(instance, &gpuCount, gpus.data());

	VkPhysicalDevice gpu = gpus[0]; // Pick the first GPU


	// 3. Create Logical Device and queue
	uint32_t queueFamilyIndex = 0; // Assume the first queue family supports graphics and presentation
    float queuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueInfo { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
    queueInfo.queueFamilyIndex = queueFamilyIndex;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &queuePriority;

	const char* deviceExtensions[] = { "VK_KHR_swapchain" }; // Required for swapchain support

    VkPhysicalDeviceDynamicRenderingFeatures dynamicFeature { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES };
	dynamicFeature.dynamicRendering = true; // Enable dynamic rendering

    VkDeviceCreateInfo deviceInfo { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
    deviceInfo.pNext = &dynamicFeature;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = _countof(deviceExtensions);
    deviceInfo.ppEnabledExtensionNames = deviceExtensions;

    VkDevice device;
	vkCreateDevice(gpu, &deviceInfo, nullptr, &device); // Create the logical device

    VkQueue queue;
    vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue);


    // 4. Surface (Win32 handle)
    VkSurfaceKHR surface;
    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    surfaceCreateInfo.hwnd = hwnd;
    surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);
    vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);

    // 5. Swapchain
    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &caps);

    VkSurfaceFormatKHR format { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
    VkSwapchainCreateInfoKHR swapInfo { VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
    swapInfo.surface = surface;
    swapInfo.minImageCount = 3;
    swapInfo.imageFormat = format.format;
    swapInfo.imageColorSpace = format.colorSpace;
    swapInfo.imageExtent = caps.currentExtent;
    swapInfo.imageArrayLayers = 1;
    swapInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapInfo.preTransform = caps.currentTransform;
    swapInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    swapInfo.clipped = true;

    VkSwapchainKHR swapchain;
    vkCreateSwapchainKHR(device, &swapInfo, nullptr, &swapchain);

    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, nullptr);
    std::vector<VkImage> images(imageCount);
    vkGetSwapchainImagesKHR(device, swapchain, &imageCount, images.data());

    // 6. Image Views
	std::vector<VkImageView> views(imageCount); // Create image views for each swapchain image
    for (uint32_t i = 0; i < imageCount; ++i)
    {
        VkImageViewCreateInfo viewInfo { VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
        viewInfo.image = images[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format.format;
        viewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		vkCreateImageView(device, &viewInfo, nullptr, &views[i]); // Create image view for the swapchain image
    }

	// 7. Command Pool + cmd buffer
    VkCommandPoolCreateInfo poolInfo { VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };
	poolInfo.queueFamilyIndex = queueFamilyIndex; // Use the same queue family index as the device queue

    VkCommandPool pool;
    vkCreateCommandPool(device, &poolInfo, nullptr, &pool);

    VkCommandBufferAllocateInfo allocInfo { VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
    allocInfo.commandPool = pool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer cmd;
	vkAllocateCommandBuffers(device, &allocInfo, &cmd); // Allocate a command buffer from the command pool

    // 8. Sync
	VkSemaphore imageAvailable; // Semaphore to signal when an image is available for rendering
	VkSemaphore renderFinished; // Semaphore to signal when rendering is finished
    VkSemaphoreCreateInfo semInfo { VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO };
    vkCreateSemaphore(device, &semInfo, nullptr, &imageAvailable);
    vkCreateSemaphore(device, &semInfo, nullptr, &renderFinished);

	VkFence fence; // Fence to synchronize command buffer execution
    VkFenceCreateInfo fenceInfo { VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };
    vkCreateFence(device, &fenceInfo, nullptr, &fence);


    while (!glfwWindowShouldClose(window))
    {
		glfwPollEvents(); // Poll for window events

		// 9. Acquire next image and record commands

        uint32_t imageIndex;
		vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAvailable, nullptr, &imageIndex); // Acquire the next image from the swapchain

        VkCommandBufferBeginInfo begin { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		vkBeginCommandBuffer(cmd, &begin); // Begin recording commands into the command buffer

        VkRenderingAttachmentInfo colorAttachment { VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO };
		colorAttachment.imageView = views[imageIndex]; // Use the image view for the current swapchain image
        colorAttachment.imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.clearValue.color = { {0.0f, 0.2f, 0.4f, 1.0f} };

        VkRenderingInfo renderInfo { VK_STRUCTURE_TYPE_RENDERING_INFO };
        renderInfo.renderArea.extent = caps.currentExtent;
        renderInfo.layerCount = 1;
        renderInfo.colorAttachmentCount = 1;
		renderInfo.pColorAttachments = &colorAttachment; // Set up dynamic rendering with a single color attachment

		vkCmdBeginRendering(cmd, &renderInfo); // Begin rendering with dynamic rendering

        // Here you would record your rendering commands
        // For example, you could bind a pipeline and draw commands here

		vkCmdEndRendering(cmd); // End the rendering commands
		vkEndCommandBuffer(cmd); // End recording commands into the command buffer

        VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submit { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &imageAvailable;
        submit.pWaitDstStageMask = &waitStage;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cmd;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &renderFinished; 

		vkResetFences(device, 1, &fence); // Reset the fence before submitting
		vkQueueSubmit(queue, 1, &submit, fence); // Submit the command buffer to the queue
        vkWaitForFences(device, 1, &fence, true, UINT64_MAX); //

        VkPresentInfoKHR present { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        present.waitSemaphoreCount = 1;
        present.pWaitSemaphores = &renderFinished;
        present.swapchainCount = 1;
        present.pSwapchains = &swapchain;
        present.pImageIndices = &imageIndex;
		vkQueuePresentKHR(queue, &present); // Present the rendered image to the swapchain
    }

    vkDeviceWaitIdle(device);
    for (auto view : views)
        vkDestroyImageView(device, view, nullptr);

    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyCommandPool(device, pool, nullptr);
    vkDestroySemaphore(device, imageAvailable, nullptr);
    vkDestroySemaphore(device, renderFinished, nullptr);
    vkDestroyFence(device, fence, nullptr);
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
    glfwDestroyWindow(window);
    glfwTerminate();
}