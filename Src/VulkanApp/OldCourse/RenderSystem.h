#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include <glfw3.h>
#include <glfw3native.h>
#include <windows.h>


namespace MiniGame
{
	class RenderSystem
	{
	public:
		uint32_t width;
		uint32_t height;
		GLFWwindow* window;
		void Run()
		{
			width = 1200;
			height = 820;
			// Initialize GLFW and create a window
			initWindow();
			initVulkan();
			Loop();
			Cleanup();
		}

		void initVulkan() 
		{
			HWND hwnd = glfwGetWin32Window(window); // Get the Win32 HWND from GLFW

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
			// Initialize Vulkan and other resources here...
		}
	};
}
