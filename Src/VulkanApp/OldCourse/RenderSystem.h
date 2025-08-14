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
		VkPhysicalDevice gpu = VK_NULL_HANDLE;


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
