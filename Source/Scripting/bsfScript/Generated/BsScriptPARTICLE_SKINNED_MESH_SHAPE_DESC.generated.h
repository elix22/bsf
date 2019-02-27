//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../Foundation/bsfCore/Particles/BsParticleEmitter.h"
#include "../../../Foundation/bsfCore/Particles/BsParticleEmitter.h"

namespace bs
{
	struct __PARTICLE_SKINNED_MESH_SHAPE_DESCInterop
	{
		ParticleEmitterMeshType type;
		bool sequential;
		MonoObject* renderable;
	};

	class BS_SCR_BE_EXPORT ScriptPARTICLE_SKINNED_MESH_SHAPE_DESC : public ScriptObject<ScriptPARTICLE_SKINNED_MESH_SHAPE_DESC>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "ParticleSkinnedMeshShapeOptions")

		static MonoObject* box(const __PARTICLE_SKINNED_MESH_SHAPE_DESCInterop& value);
		static __PARTICLE_SKINNED_MESH_SHAPE_DESCInterop unbox(MonoObject* value);
		static PARTICLE_SKINNED_MESH_SHAPE_DESC fromInterop(const __PARTICLE_SKINNED_MESH_SHAPE_DESCInterop& value);
		static __PARTICLE_SKINNED_MESH_SHAPE_DESCInterop toInterop(const PARTICLE_SKINNED_MESH_SHAPE_DESC& value);

	private:
		ScriptPARTICLE_SKINNED_MESH_SHAPE_DESC(MonoObject* managedInstance);

	};
}
