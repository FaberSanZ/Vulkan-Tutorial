// VulkanApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <vector>
#include "vulkan/vulkan.h"

struct Vector2
{
	float x, y;

	Vector2() : x(0), y(0) {}
	
	Vector2(float x, float y) : x(x), y(y) {}
	Vector2 operator+(const Vector2& other) const { return Vector2(x + other.x, y + other.y); }
	Vector2 operator-(const Vector2& other) const { return Vector2(x - other.x, y - other.y); }
	Vector2 operator*(float scalar) const { return Vector2(x * scalar, y * scalar); }
};

VkInstance instance;
VkPhysicalDevice gpu;

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

	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

}


void SelectGPU(uint32_t index)
{
	// This function would typically create a Vulkan device and select a physical GPU.
	// For simplicity, we are not implementing this in this example.

	uint32_t physicalDeviceCount = 0;
	VkResult result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);

	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

	result = vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

	if(physicalDeviceCount == 0 && index >= physicalDeviceCount) 
	{
		std::cerr << "No physical devices found or index out of range." << std::endl;
		return;
	}

	gpu = physicalDevices[index]; // Select the first physical device
}	



void InitializeVulkan() 
{
	CreateInstance();
	SelectGPU(0);
}

void Destroy()
{
	if (instance != VK_NULL_HANDLE) {
		vkDestroyInstance(instance, nullptr);
	}
}

int main()
{
	InitializeVulkan();
	Destroy();
}


