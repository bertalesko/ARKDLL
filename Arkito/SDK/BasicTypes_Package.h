﻿#pragma once

/**
 * Name: Arkito
 * Version: 1
 */

#define UE4
#define GOBJECTS_CHUNKS

#ifdef _MSC_VER
	#pragma pack(push, 0x01)
#endif

namespace CG
{
	// --------------------------------------------------
	// # Forwards
	// --------------------------------------------------
	class UObject;
	// --------------------------------------------------
	// # Global functions
	// --------------------------------------------------
	bool InitSdk(const std::wstring& moduleName, uintptr_t gObjectsOffset, uintptr_t gNamesOffset, uintptr_t gWorldOffset);
	bool InitSdk();
	void tick();
	bool bartHook();
	//bool HookDetour();
	void prep();
	
	/*void hooked_postrender(UGameViewportClient* thisptr, UCanvas* canvas);
	void PostRender();
	void _PostRender(UShooterGameViewportClient* viewport, UCanvas* canvas);*/
	template<typename Fn>
	Fn GetVFunction(const void* instance, size_t index)
	{
		auto vtable = *static_cast<const void***>(const_cast<void*>(instance));
		return reinterpret_cast<Fn>(const_cast<void (*)>(vtable[index]));
	}

}

#ifdef _MSC_VER
	#pragma pack(pop)
#endif

#include "BasicTypes_Structs.h"
#include "BasicTypes_Classes.h"
