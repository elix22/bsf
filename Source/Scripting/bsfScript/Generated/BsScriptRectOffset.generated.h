//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "../../../Foundation/bsfEngine/Utility/BsRectOffset.h"

namespace bs
{
	class BS_SCR_BE_EXPORT ScriptRectOffset : public ScriptObject<ScriptRectOffset>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "RectOffset")

		static MonoObject* box(const RectOffset& value);
		static RectOffset unbox(MonoObject* value);

	private:
		ScriptRectOffset(MonoObject* managedInstance);

	};
}
