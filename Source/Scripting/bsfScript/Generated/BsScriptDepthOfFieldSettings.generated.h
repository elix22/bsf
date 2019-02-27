//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptReflectable.h"
#include "../../../Foundation/bsfCore/Renderer/BsRenderSettings.h"

namespace bs
{
	struct DepthOfFieldSettings;

	class BS_SCR_BE_EXPORT ScriptDepthOfFieldSettings : public TScriptReflectable<ScriptDepthOfFieldSettings, DepthOfFieldSettings>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "DepthOfFieldSettings")

		ScriptDepthOfFieldSettings(MonoObject* managedInstance, const SPtr<DepthOfFieldSettings>& value);

		static MonoObject* create(const SPtr<DepthOfFieldSettings>& value);

	private:
		static void Internal_DepthOfFieldSettings(MonoObject* managedInstance);
		static bool Internal_getenabled(ScriptDepthOfFieldSettings* thisPtr);
		static void Internal_setenabled(ScriptDepthOfFieldSettings* thisPtr, bool value);
		static float Internal_getfocalDistance(ScriptDepthOfFieldSettings* thisPtr);
		static void Internal_setfocalDistance(ScriptDepthOfFieldSettings* thisPtr, float value);
		static float Internal_getfocalRange(ScriptDepthOfFieldSettings* thisPtr);
		static void Internal_setfocalRange(ScriptDepthOfFieldSettings* thisPtr, float value);
		static float Internal_getnearTransitionRange(ScriptDepthOfFieldSettings* thisPtr);
		static void Internal_setnearTransitionRange(ScriptDepthOfFieldSettings* thisPtr, float value);
		static float Internal_getfarTransitionRange(ScriptDepthOfFieldSettings* thisPtr);
		static void Internal_setfarTransitionRange(ScriptDepthOfFieldSettings* thisPtr, float value);
		static float Internal_getnearBlurAmount(ScriptDepthOfFieldSettings* thisPtr);
		static void Internal_setnearBlurAmount(ScriptDepthOfFieldSettings* thisPtr, float value);
		static float Internal_getfarBlurAmount(ScriptDepthOfFieldSettings* thisPtr);
		static void Internal_setfarBlurAmount(ScriptDepthOfFieldSettings* thisPtr, float value);
	};
}
