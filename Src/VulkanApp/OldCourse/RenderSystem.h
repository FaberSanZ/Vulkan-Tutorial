#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <glfw3.h>
#include <glfw3native.h>
#include <windows.h>
#include <iostream>
#include <vector>

namespace MiniGame
{
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}



	class RenderSystem
	{
	public:
		uint32_t width;
		uint32_t height;
		GLFWwindow* window;
		const std::vector<const char*> validationLayers = { "VK_LAYER_KHRONOS_validation" };
		const bool enableValidationLayers = true;

		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkPhysicalDevice gpu;
		VkDevice device;
		VkQueue queue;
		VkSurfaceKHR surface;
		VkSwapchainKHR swapchain;


		void Run()
		{
			width = 1200;
			height = 820;
			// Initialize GLFW and create a window
			initWindow();
			initVulkan();
			Loop();
			Cleanup();


			std::cout << "Vulkan instance: " << instance;
		}

		void initVulkan() 
		{
			HWND hwnd = glfwGetWin32Window(window); // Get the Win32 HWND from GLFW

			VkApplicationInfo appInfo { VK_STRUCTURE_TYPE_APPLICATION_INFO };
			appInfo.apiVersion = VK_API_VERSION_1_3; // Use Vulkan 1.3

			const char* extensions[] = { "VK_KHR_surface", "VK_KHR_win32_surface" };
			
			VkInstanceCreateInfo instanceInfo { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
			instanceInfo.pApplicationInfo = &appInfo;
			instanceInfo.enabledExtensionCount = 2;
			instanceInfo.ppEnabledExtensionNames = extensions;



			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
			if (enableValidationLayers)
			{
				instanceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				instanceInfo.ppEnabledLayerNames = validationLayers.data();

				populateDebugMessengerCreateInfo(debugCreateInfo);
				instanceInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
			}
			else
			{
				instanceInfo.enabledLayerCount = 0;
				instanceInfo.pNext = nullptr;
			}


			vkCreateInstance(&instanceInfo, nullptr, &instance); // Create Vulkan instance


			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
			std::vector<VkPhysicalDevice> gpus(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, gpus.data());

			gpu = gpus[0]; // Pick the first GPU (you can add logic to choose a suitable GPU based on your requirements)


			float queuePriority = 1.0f;	
			uint32_t queueFamilyIndex = 0; // Assume the first queue family supports graphics and presentation
			VkDeviceQueueCreateInfo queueInfo { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO };
			queueInfo.queueFamilyIndex = queueFamilyIndex;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &queuePriority;

			const char* deviceExtensions[] = { "VK_KHR_swapchain" }; // Required for swapchain support


			VkDeviceCreateInfo deviceInfo { VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };
			deviceInfo.pNext = nullptr; // No additional features needed for this example
			deviceInfo.queueCreateInfoCount = 1;
			deviceInfo.pQueueCreateInfos = &queueInfo;
			deviceInfo.enabledExtensionCount = static_cast<uint32_t>(std::size(deviceExtensions));
			deviceInfo.ppEnabledExtensionNames = deviceExtensions;

			if (enableValidationLayers)
			{
				deviceInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				deviceInfo.ppEnabledLayerNames = validationLayers.data();
			}
			else
				deviceInfo.enabledLayerCount = 0;

			vkCreateDevice(gpu, &deviceInfo, nullptr, &device);

			vkGetDeviceQueue(device, queueFamilyIndex, 0, &queue); // Get the graphics queue from the device



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

			vkCreateSwapchainKHR(device, &swapInfo, nullptr, &swapchain);



		}



		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
		{
			createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = debugCallback;
		}

		void setupDebugMessenger()
		{
			if (!enableValidationLayers) return;

			VkDebugUtilsMessengerCreateInfoEXT createInfo;
			populateDebugMessengerCreateInfo(createInfo);

			if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to set up debug messenger!");
			}
		}


		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
		{
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

			return VK_FALSE;
		}



		void Loop()
		{
			while (!glfwWindowShouldClose(window))
			{
				glfwPollEvents(); // Poll for window events
				// Here you would handle rendering, command buffer submission, etc.
			}
		}


		void Cleanup()
		{
			// Cleanup Vulkan resources, GLFW window, etc.
			glfwDestroyWindow(window);
			glfwTerminate();
		}


		void initWindow()
		{
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
			window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
		}
	};
}
