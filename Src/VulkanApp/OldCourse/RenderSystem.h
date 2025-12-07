

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

		Float3() : x(0), y(0), z(0) {}
		Float3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	};
	struct Float4x4
	{
		float m[4][4];

		Float4x4() 
		{
			for (int i = 0; i < 4; ++i)
				for (int j = 0; j < 4; ++j)
					m[i][j] = (i == j) ? 1.0f : 0.0f; // Identity matrix
		}

		float* operator[](int row) 
		{
			return m[row];
		}
	};


	struct Int2
	{
		int x;
		int y;

		Int2() : x(0), y(0) {}
		Int2(int _x, int _y) : x(_x), y(_y) {}
	};

	struct Int3
	{
		int x;
		int y;
		int z;

		Int3() : x(0), y(0), z(0) {}
		Int3(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
	};

}