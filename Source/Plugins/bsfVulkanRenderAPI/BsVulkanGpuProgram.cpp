//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsVulkanGpuProgram.h"
#include "BsVulkanRenderAPI.h"
#include "BsVulkanDevice.h"
#include "BsVulkanUtility.h"
#include "RenderAPI/BsGpuParams.h"
#include "RenderAPI/BsGpuParamDesc.h"
#include "Managers/BsGpuProgramManager.h"
#include "RenderAPI/BsVertexDeclaration.h"
#include "Managers/BsHardwareBufferManager.h"
#include "Profiling/BsRenderStats.h"
#include "FileSystem/BsFileSystem.h"
#include "FileSystem/BsDataStream.h"

#if BS_PLATFORM == BS_PLATFORM_OSX
	#include <MoltenVK/vk_mvk_moltenvk.h>
#endif

namespace bs { namespace ct
{
	VulkanShaderModule::VulkanShaderModule(VulkanResourceManager* owner, VkShaderModule module)
		:VulkanResource(owner, true), mModule(module)
	{ }

	VulkanShaderModule::~VulkanShaderModule()
	{
		vkDestroyShaderModule(mOwner->getDevice().getLogical(), mModule, gVulkanAllocator);
	}

	VulkanGpuProgram::VulkanGpuProgram(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
		: GpuProgram(desc, deviceMask), mDeviceMask(deviceMask), mModules()
	{

	}

	VulkanGpuProgram::~VulkanGpuProgram()
	{
		for (UINT32 i = 0; i < BS_MAX_DEVICES; i++)
		{
			if (mModules[i] != nullptr)
				mModules[i]->destroy();
		}

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_GpuProgram);
	}

	void VulkanGpuProgram::initialize()
	{
		if (!isSupported())
		{
			mIsCompiled = false;
			mCompileMessages = "Specified program is not supported by the current render system.";

			GpuProgram::initialize();
			return;
		}

		if(!mBytecode ||
#if BS_PLATFORM == BS_PLATFORM_OSX
			mBytecode->compilerId != MOLTENVK_COMPILER_ID || mBytecode->compilerVersion != MOLTENVK_COMPILER_VERSION)
#else
			mBytecode->compilerId != VULKAN_COMPILER_ID || mBytecode->compilerVersion != VULKAN_COMPILER_VERSION)
#endif
		{
			GPU_PROGRAM_DESC desc;
			desc.type = mType;
			desc.entryPoint = mEntryPoint;
#if BS_PLATFORM == BS_PLATFORM_OSX
			desc.language = "mvksl";
#else
			desc.language = "vksl";
#endif
			desc.source = mSource;

			mBytecode = compileBytecode(desc);
		}

		mCompileMessages = mBytecode->messages;
		mIsCompiled = mBytecode->instructions.data != nullptr;

		if(mIsCompiled)
		{
			VulkanRenderAPI& rapi = static_cast<VulkanRenderAPI&>(RenderAPI::instance());
			VulkanDevice* devices[BS_MAX_DEVICES];

			UINT32 codeSize = mBytecode->instructions.size;
			UINT8* code = mBytecode->instructions.data;

#if BS_PLATFORM == BS_PLATFORM_OSX
			UINT32 workgroupSize[3] = { 1, 1, 1 };
			if(mType == GPT_COMPUTE_PROGRAM)
			{
				assert(codeSize > sizeof(workgroupSize));

				memcpy(workgroupSize, code, sizeof(workgroupSize));
				code += sizeof(workgroupSize);
				codeSize -= sizeof(workgroupSize);
			}
#endif

			// Create Vulkan module
			VkShaderModuleCreateInfo moduleCI;
			moduleCI.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleCI.pNext = nullptr;
			moduleCI.flags = 0;
			moduleCI.codeSize = codeSize;
			moduleCI.pCode = (uint32_t*)code;

			VulkanUtility::getDevices(rapi, mDeviceMask, devices);

			for (UINT32 i = 0; i < BS_MAX_DEVICES; i++)
			{
				if (devices[i] != nullptr)
				{
					VkDevice vkDevice = devices[i]->getLogical();
					VulkanResourceManager& rescManager = devices[i]->getResourceManager();

					VkShaderModule shaderModule;
					VkResult result = vkCreateShaderModule(vkDevice, &moduleCI, gVulkanAllocator, &shaderModule);
					assert(result == VK_SUCCESS);

#if BS_PLATFORM == BS_PLATFORM_OSX
					if(mType == GPT_COMPUTE_PROGRAM)
						vkSetWorkgroupSizeMVK(shaderModule, workgroupSize[0], workgroupSize[1], workgroupSize[2]);
#endif
					mModules[i] = rescManager.create<VulkanShaderModule>(shaderModule);
				}
			}

			mParametersDesc = mBytecode->paramDesc;

			if (mType == GPT_VERTEX_PROGRAM)
			{
				mInputDeclaration = HardwareBufferManager::instance().createVertexDeclaration(
					mBytecode->vertexInput, mDeviceMask);
			}
		}

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_GpuProgram);

		GpuProgram::initialize();
	}
}}
