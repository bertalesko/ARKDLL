#include "pch.h"
#include "memoryUtils.h"




//#define W2S(WorldLocation, ScreenLocation, bPlayerViewPortRelative) if(!myController->ProjectWorldLocationToScreen(WorldLocation, ScreenLocation, bPlayerViewPortRelative)) std::cout << "WorldToScreen failed at line " << __LINE__ << "\n"

//CG::UFont* font = 

namespace ESP
{


	auto module_base = (uintptr_t)GetModuleHandle(nullptr);

	uintptr_t postRenderAddress = reinterpret_cast<uintptr_t>(GetModuleHandle(0)) + PatternScan(module_base,"E8 ? ? ? ? C6 44 24 ? ? 8B 05 ? ? ? ?");
	auto PostRender = reinterpret_cast<void(*)(CG::UGameViewportClient*, CG::UCanvas*)>(postRenderAddress);

	//inline void CanvasDrawText(CG::UCanvas* canvas, const CG::FString& renderString, const CG::FVector2D& screenPosition, const CG::FLinearColor& color, CG::FVector2D scale = { 1.0f, 1.0f })
	//{
	//	return canvas->K2_DrawText(GEngine->SubtitleFont, renderString, screenPosition, scale, color, false, { 0.0f, 0.0f, 0.0f, 0.0f, }, scale, true, true, true, { 0.0f, 0.0f, 0.0f, 1.0f });
	//}

	//inline void DrawCrosshairToHead(CG::UCanvas* canvas, CG::ACharacter* enemy, int headBoneIndex, CG::FLinearColor drawColor = { 255.0f, 0.0f, 0.0f, 1.0f })
	//{

	//	CG::FVector2D vecPostProject;

	//	W2S(enemy->Mesh->GetBoneMatrix(29).WPlane, &vecPostProject, true);

	//	canvas->K2_DrawLine({ 960.0f, 540.0f }, vecPostProject, 1.0f, drawColor);
	//}

	inline void DrawLineBetweenBones(CG::UCanvas* canvas, CG::ACharacter* character, int index, int otheIndex)
	{
		if (character->Mesh->GetNumBones() < index && character->Mesh->GetNumBones() < otheIndex)
		{
			return;
		}

		CG::FVector vec1PreProject, vec2PreProject;
		CG::FVector2D vec1PostProject, vec2PostProject;

		vec1PreProject = character->Mesh->GetBoneMatrix(index).WPlane;
		vec2PreProject = character->Mesh->GetBoneMatrix(otheIndex).WPlane;

		myController->ProjectWorldLocationToScreen(vec1PreProject, &vec1PostProject, true);
		myController->ProjectWorldLocationToScreen(vec2PreProject, &vec2PostProject, true);

		canvas->K2_DrawLine(vec1PostProject, vec2PostProject, 1.0f, { 255.0f, 0.0f, 0.0f, 1.0f });
	}

	inline void DrawBox(CG::UCanvas* canvas, CG::ACharacter* character)
	{
		// X - Left/Right
		// Y - Depth
		// Z - Up/Down
// 
//	   z|	  y
//		|	 /
//		|	/
//		|  /
//		| /
//		|/______________x 
//

		CG::FBoxSphereBounds bounds = character->Mesh->CachedWorldSpaceBounds;

		CG::FVector f1, f2, f3, f4, b1, b2, b3, b4;
		CG::FVector2D w2s_f1, w2s_f2, w2s_f3, w2s_f4, w2s_b1, w2s_b2, w2s_b3, w2s_b4;

		const float size_X = 80;
		const float size_Y = bounds.BoxExtent.Y * 2;
		const float size_Z = character->BaseEyeHeight * 2;

		CG::FVector baseVector;

		baseVector.X = bounds.Origin.X + 40;
		baseVector.Y = bounds.Origin.Y + bounds.BoxExtent.Y;
		baseVector.Z = bounds.Origin.Z - character->BaseEyeHeight;

		f1.X = f4.X = b1.X = b4.X = baseVector.X;
		f2.X = f3.X = b2.X = b3.X = baseVector.X - size_X;

		f1.Y = f2.Y = f3.Y = f4.Y = baseVector.Y;
		b1.Y = b2.Y = b3.Y = b4.Y = baseVector.Y - size_Y;

		f1.Z = f2.Z = b1.Z = b2.Z = baseVector.Z;
		f3.Z = f4.Z = b3.Z = b4.Z = baseVector.Z + size_Z;



		/*
		f1.X = f4.X = b1.X = b4.X = bounds.Origin.X;
		f2.X = f3.X = b2.X = b3.X = bounds.Origin.X - X_extend;

		f1.Y = f2.Y = f3.Y = f4.Y = bounds.Origin.Y;
		b1.Y = b2.Y = b3.Y = b4.Y = bounds.Origin.Y - Y_extend;

		f1.Z = f2.Z = b1.Z = b2.Z = bounds.Origin.Z - character->BaseEyeHeight;
		f3.Z = f4.Z = b3.Z = b4.Z = bounds.Origin.Z + character->BaseEyeHeight;
		*/

		W2S(f1, &w2s_f1, true);
		W2S(f2, &w2s_f2, true);
		W2S(f3, &w2s_f3, true);
		W2S(f4, &w2s_f4, true);

		W2S(b1, &w2s_b1, true);
		W2S(b2, &w2s_b2, true);
		W2S(b3, &w2s_b3, true);
		W2S(b4, &w2s_b4, true);

		//Front
		canvas->K2_DrawLine(w2s_f1, w2s_f2, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });
		canvas->K2_DrawLine(w2s_f2, w2s_f3, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });
		canvas->K2_DrawLine(w2s_f3, w2s_f4, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });
		canvas->K2_DrawLine(w2s_f4, w2s_f1, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });

		//Back
		canvas->K2_DrawLine(w2s_b1, w2s_b2, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });
		canvas->K2_DrawLine(w2s_b2, w2s_b3, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });
		canvas->K2_DrawLine(w2s_b3, w2s_b4, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });
		canvas->K2_DrawLine(w2s_b4, w2s_b1, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });

		//Connection
		canvas->K2_DrawLine(w2s_f1, w2s_b1, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });
		canvas->K2_DrawLine(w2s_f2, w2s_b2, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });
		canvas->K2_DrawLine(w2s_f3, w2s_b3, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });
		canvas->K2_DrawLine(w2s_f4, w2s_b4, 1.5f, { 255.0f, 0.0f, 0.0f, 1.0f });

		//bounds.Origin.Print("bounds.Origin");
		//bounds.BoxExtent.Print("bounds.BoxExtent");
	}

	void PostRenderHook(CG::UGameViewportClient* thisPtr, CG::UCanvas* canvas)
	{


		return PostRender(thisPtr, canvas);
	}

	inline void InitPRHook()
	{
		DWORD virtualProtect;
		void** vftable = *static_cast<void***>(static_cast<void*>(GEngine->GameViewport));

		VirtualProtect((&vftable[0x62]), 0x8, PAGE_EXECUTE_READWRITE, &virtualProtect);

		vftable[0x62] = PostRenderHook;

		VirtualProtect((&vftable[0x62]), 0x8, virtualProtect, &virtualProtect);
	}
};