

#include <vulkan/vulkan.h>
#include <glfw3.h>
#include <glfw3native.h>

static int render() 
{
	uint32_t width = 1200;
	uint32_t height = 820;

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(width, height, "Vulkan 2D Game", nullptr, nullptr);

	VkInstance instance;


	while (!glfwWindowShouldClose(window)) 
	{
		glfwPollEvents();
	}

	glfwDestroyWindow(window); // Clean up and destroy the window
	glfwTerminate(); // Clean up and close GLFW
	return 0;

}


namespace GameMath 
{
	struct Float2
	{
		float x;
		float y;

		Float2() : x(0), y(0) {}
		Float2(float _x, float _y) : x(_x), y(_y) {}
	};
	struct Float3
	{
		float x;
		float y;
		float z;
	};
	struct Float4x4
	{
		float m[4][4];
	};


	struct Int2
	{
		int x;
		int y;
	};

	struct Int3
	{
		int x;
		int y;
		int z;
	};

}