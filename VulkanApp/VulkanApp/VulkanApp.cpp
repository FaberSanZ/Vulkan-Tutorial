// VulkanApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "vulkan/vulkan.h"



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

	VkInstance instance;
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
	

	

	// Clean up
	vkDestroyInstance(instance, nullptr);
}

void InitializeVulkan() 
{
	CreateInstance();
}
int main()
{
	InitializeVulkan();
}


