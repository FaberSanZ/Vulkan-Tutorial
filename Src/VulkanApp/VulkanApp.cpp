// VulkanApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "vulkan/vulkan.h"
#include <glfw3.h>

struct Vector2
{
	float x, y;

	Vector2() : x(0), y(0) {}
	
	Vector2(float x, float y) : x(x), y(y) {}
	Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
	Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
	Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
};

struct QueueFamilyIndices
{
	uint32_t index { 0 }; // default family index for graphics queue and compute queue
};

VkInstance instance;
VkPhysicalDevice gpu;
VkDevice device;
VkPhysicalDeviceFeatures deviceFeatures {};
VkQueue queue;


void CreateInstance()
{
    VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

	// extensions for Win32 platform
	// these are required for Vulkan to work on Windows
	// TODO: add extensions for other platforms
	const char* instanceExtensions[] =
	{
		"VK_KHR_surface",
		"VK_KHR_win32_surface"
	};

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = sizeof(instanceExtensions) / sizeof(instanceExtensions[0]);
	createInfo.ppEnabledExtensionNames = instanceExtensions;

	vkCreateInstance(&createInfo, nullptr, &instance);

}


void SelectGPU(uint32_t index)
{
	// This function would typically create a Vulkan device and select a physical GPU.
	// For simplicity, we are not implementing this in this example.

	uint32_t physicalDeviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

	vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());


	gpu = physicalDevices[index]; // Select the first physical device
}	

void CreateDevice()
{
	// This function would typically create a Vulkan logical device.
	// For simplicity, we are not implementing this in this example.
	// You would need to query the queue family indices and create a VkDevice.
	QueueFamilyIndices queueIndices {};
	float queuePriority = 1.0f;


	VkDeviceQueueCreateInfo queueCreateInfo {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = queueIndices.index;
	queueCreateInfo.queueCount = 1;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	VkDeviceCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;


	vkCreateDevice(gpu, &createInfo, nullptr, &device);


	vkGetDeviceQueue(device, queueIndices.index, 0, &queue);
}


void InitializeVulkan() 
{
	CreateInstance();
	SelectGPU(0); // Select the first GPU (index 0)
	CreateDevice();

	std::cout << "Instance: " << instance << std::endl;
	std::cout << "gpu: " << gpu << std::endl;
	std::cout << "device: " << device << std::endl;
	std::cout << "queue: " << queue << std::endl;
}

void Destroy()
{
	if (instance != VK_NULL_HANDLE) {
		vkDestroyInstance(instance, nullptr);
	}
}

int main()
{
	uint32_t width = 800;
	uint32_t height = 600;


	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	GLFWwindow* window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);


	InitializeVulkan();



	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
	}

	Destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
}


