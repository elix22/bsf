//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsGLPrerequisites.h"
#include "RenderAPI/BsCommandBuffer.h"
#include "BsGLRenderAPI.h"

namespace bs { namespace ct
{
	/** @addtogroup GL
	 *  @{
	 */

	/**
	 * Command buffer implementation for OpenGL, which doesn't support multi-threaded command generation. Instead all
	 * commands are stored in an internal buffer, and then sent to the actual render API when the buffer is executed.
	 */
	class GLCommandBuffer : public CommandBuffer
	{
	public:
		/** Registers a new command in the command buffer. */
		void queueCommand(const std::function<void()> command);

		/** Executes all commands in the command buffer. Not supported on secondary buffer. */
		void executeCommands();

		/** @copydoc CommandBuffer::getState() */
		CommandBufferState getState() const override;

		/** @copydoc CommandBuffer::reset() */
		void reset() override;
		
	private:
		friend class GLCommandBufferManager;
		friend class GLRenderAPI;

		GLCommandBuffer(GpuQueueType type, UINT32 deviceIdx, UINT32 queueIdx, bool secondary);

		/** Returns true if the command buffer has finished executing on the GPU. */
		bool isComplete() const;

		GLsync mFence = 0;
		bool mCommandQueued = false;
		bool mIsSubmitted = false;
	};

	/** @} */
}}
