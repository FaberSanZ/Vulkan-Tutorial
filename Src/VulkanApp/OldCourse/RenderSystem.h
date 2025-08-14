#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <glfw3.h>
#include <glfw3native.h>
#include <windows.h>
#include <iostream>


namespace MiniGame
{
	class RenderSystem
	{
	public:
		uint32_t width;
		uint32_t height;
		GLFWwindow* window;


		VkInstance instance;

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

			vkCreateInstance(&instanceInfo, nullptr, &instance); // Create Vulkan instance

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
