#pragma once
#include "pch.h"



//#include "memoryUtils.h"
//#include "include/detours.h"
//#include "include/detver.h"
//#pragma comment(lib, "./libz/detours.lib")

#define VERSION = "f2"

 /* some RGB color definitions                                                 */

namespace CG
{
	// --------------------------------------------------
	// # Structs Static Fields
	// --------------------------------------------------
	TNameEntryArray*                                            FName::GNames = nullptr;                                 // 0x0000(0x0000)
	
	// --------------------------------------------------
	// # Global functions
	// --------------------------------------------------
	/**
	 * Initialize SDK
	 */
	//auto module_base = (uintptr_t)GetModuleHandle(nullptr);
	auto module_base = (uintptr_t)GetModuleHandle(nullptr);
	//uintptr_t postRenderAddress = reinterpret_cast<uintptr_t>(GetModuleHandle(0)) + PatternScan(module_base, "E8 ? ? ? ? C6 44 24 ? ? 8B 05 ? ? ? ?");
	//auto PostRender = reinterpret_cast<void(*)(CG::UGameViewportClient*, CG::UCanvas*)>(postRenderAddress);
	bool udane = 0;
	Settings _settings;

	
	//uintptr_t _PostRender = PatternScan(module_base,"E8 ? ? ? ? C6 44 24 ? ? 8B 05 ? ? ? ?");
	//auto PostRender = reinterpret_cast<void(*)(CG::UGameViewportClient*, CG::UCanvas*)>(postRenderAddress);

	//void HookedPostRender(UGameViewportClient* thisPointer, UCanvas* canvas);
	

	
	CG::UWorld* GWorldz = nullptr;

	

	typedef void(__thiscall* tPostRender)(UGameViewportClient* uObject, UCanvas* Canvas);
	tPostRender OriginalPostRender;












	CG::UFont* font = nullptr;

	//Deklaracja podstawionej funkcji funkcji
	void hooked_postrender(CG::UGameViewportClient* thisptr, CG::UCanvas* canvas);

	//Adres oryginalnej funkcji
	void(__thiscall* original_postrender)(CG::UGameViewportClient*, CG::UCanvas*) = nullptr;

	bool HookDetour()
	{


		//Wczytaj font, -- TODO : Zmienic font na cos bardziej czytelnego
		font = CG::UObject::FindObject<CG::UFont>(("Font Roboto18.Roboto18"));



		CG::UWorld* World = *CG::UWorld::GWorld;


		CG::UGameInstance* game_instance_ = World->OwningGameInstance;
		std::cout << "Init +" << std::endl;

		// Pauza zeby wiedziec ze wczytalo, sprawdzic czemu ma problemy z wczytywaniem
		system("pause");
		CG::ULocalPlayer* uLocal_Player_ = game_instance_->LocalPlayers[0];
		CG::UGameViewportClient* viewportClient_ = uLocal_Player_->ViewportClient;



		PVOID* VTable = (PVOID*)viewportClient_->VfTable;

		if (viewportClient_ && viewportClient_->IsA(CG::UGameViewportClient::StaticClass()))
		{
			PVOID* VTable = (PVOID*)viewportClient_->VfTable;
			PVOID PostRenderObject = (PVOID)&VTable[0x5A];
			*(PVOID*)&original_postrender = *(PVOID*)PostRenderObject;
			/// Czemu nie dziala Detour ? -- sprawidzc
			/*	DetourTransactionBegin();
				DetourUpdateThread(GetCurrentThread());
				DetourAttach((PVOID*)original_postrender, hooked_postrender);
			*/

			DWORD Old;
			VirtualProtect(PostRenderObject, sizeof(PVOID), PAGE_READWRITE, &Old);
			*(PVOID*)&original_postrender = *(PVOID*)PostRenderObject;
			*(PVOID*)PostRenderObject = &hooked_postrender;
			VirtualProtect(PostRenderObject, sizeof(PVOID), Old, &Old);
			//inithook_postrender = true;
		}




		return 1;
	}


	//bool espStatus = vars::esp;


	//Definicja podstawionego postrendera
	void hooked_postrender(CG::UGameViewportClient* thisptr, CG::UCanvas* canvas)
	{

		if (thisptr == nullptr || canvas == nullptr)
			return original_postrender(thisptr, canvas);


		const auto& world = thisptr->World;
		if (world == nullptr || world->PersistentLevel == nullptr || world->OwningGameInstance == nullptr)
			return original_postrender(thisptr, canvas);

		const auto& local_player = thisptr->GameInstance->LocalPlayers[0];
		if (local_player == nullptr)
			return original_postrender(thisptr, canvas);

		const auto& local_controller = local_player->PlayerController;
		if (local_controller == nullptr || local_controller->Character == nullptr || local_controller->Pawn == nullptr || local_controller->Pawn->PlayerState == nullptr || local_controller->PlayerCameraManager == nullptr || local_controller->PlayerCameraManager->TransformComponent == nullptr)
			return original_postrender(thisptr, canvas);

		auto local_character = reinterpret_cast<CG::AShooterCharacter*>(local_controller->Character);
		auto local_PrimalCharacter = reinterpret_cast<CG::APrimalCharacter*>(local_controller->Character);



		CG::TTransArray<CG::AActor*> actors_array = thisptr->World->PersistentLevel->Actors;
		for (size_t j = 0; j < actors_array.Count(); j++)
		{
			const auto& actor = actors_array[j];
			if (actor == nullptr || actor->RootComponent == nullptr)
				continue;
			const wchar_t* widecstr;
			std::wstring widestr;
			if (actor->IsA(CG::AShooterCharacter::StaticClass()))
			{
				const auto character = reinterpret_cast<CG::AShooterCharacter*>(actor);
				if (character == local_character)
					continue;

				CG::APrimalCharacter* PrimCharacter = reinterpret_cast<CG::APrimalCharacter*>(actor);
				int charLevel = PrimCharacter->MyCharacterStatusComponent->BaseCharacterLevel + PrimCharacter->MyCharacterStatusComponent->ExtraCharacterLevel;
				char* string_CharLevel;
				std::string str2;
				std::stringstream  ss2;
				ss2 << charLevel;
				ss2 >> str2;
				widestr = std::wstring(str2.begin(), str2.end());
				widecstr = widestr.c_str();
				//asd->MyCharacterStatusComponent->ExtraCharacterLevel;
				CG::FVector ActorLoc = actor->K2_GetActorLocation();
				CG::FVector TempV = canvas->K2_Project(ActorLoc);
				CG::FVector2D Size{ 64.0f, 64.0f };
				CG::FVector2D Pos{ TempV.X - Size.X / 2, TempV.Y - Size.Y / 2 };
				canvas->K2_DrawBox(Pos, Size, 2.0f);
				canvas->K2_DrawText(font, character->AShooterCharacterPlayerName, { Pos.X, Pos.Y + 30 }, { 0,0,204 }, 0, { 255, 0, 0, 255 }, { 0.0 }, 0, 0, true, { 255, 0, 0, 255 });
				canvas->K2_DrawText(font, widecstr, { Pos.X, Pos.Y + 45 }, { 0,0,204 }, 0, { 255, 0, 0, 255 }, { 0.0 }, 0, 0, true, { 255, 0, 0, 255 });





			}
			if (actor->IsPrimalDino())

			{
				//Swoje wypisuj na zielono
				if (actor->IsPrimalCharFriendly(local_PrimalCharacter))
				{
					const auto character = reinterpret_cast<CG::APrimalDinoCharacter*>(actor);
					if (character == nullptr)
						continue;
					CG::APrimalCharacter* PrimCharDino = reinterpret_cast<CG::APrimalCharacter*>(actor);
					CG::FVector ActorLoc = actor->K2_GetActorLocation();
					CG::FVector TempV = canvas->K2_Project(ActorLoc);
					CG::FVector2D Size{ 64.0f, 64.0f };
					CG::FVector2D Pos{ TempV.X - Size.X / 2, TempV.Y - Size.Y / 2 };
					auto name = character->Class;
					std::string className = name->GetName();
					widestr = std::wstring(className.begin(), className.end());
					const wchar_t* widecstr = widestr.c_str();

					canvas->K2_DrawBox(Pos, Size, 2.0f);
					canvas->K2_DrawText(font, widecstr, { Pos.X, Pos.Y + 15 }, { 0,102 ,0 }, 0, { 255, 0, 0, 255 }, { 0.0 }, 0, 0, true, { 255, 0, 0, 255 });

					int charLevel = PrimCharDino->MyCharacterStatusComponent->BaseCharacterLevel + PrimCharDino->MyCharacterStatusComponent->ExtraCharacterLevel;
					char* string_CharLevel;
					std::stringstream ss;
					std::string str;
					ss << charLevel;
					ss >> str;
					widestr = std::wstring(str.begin(), str.end());
					widecstr = widestr.c_str();
				}
				//Inne wypisuj na czerwono
				else
				{
					const auto character = reinterpret_cast<CG::APrimalDinoCharacter*>(actor);
					if (character == nullptr)
						continue;
					CG::APrimalCharacter* asd = reinterpret_cast<CG::APrimalCharacter*>(actor);
					CG::FVector ActorLoc = actor->K2_GetActorLocation();
					CG::FVector TempV = canvas->K2_Project(ActorLoc);
					CG::FVector2D Size{ 64.0f, 64.0f };
					CG::FVector2D Pos{ TempV.X - Size.X / 2, TempV.Y - Size.Y / 2 };
					auto name = character->Class;
					std::string className = asd->Name.GetName();
					widestr = std::wstring(className.begin(), className.end());
					const wchar_t* classString = widestr.c_str();

					int charLevel = asd->MyCharacterStatusComponent->BaseCharacterLevel + asd->MyCharacterStatusComponent->ExtraCharacterLevel;
					char* string_CharLevel;
					std::string str;
					std::stringstream  ss2;
					ss2 << charLevel;
					ss2 >> str;
					//std::string str = std::string(string_CharLevel);
					std::wstring widestr = std::wstring(str.begin(), str.end());
					widecstr = widestr.c_str();


					//FString Fname = L"Elo";
					canvas->K2_DrawBox(Pos, Size, 2.0f);
					canvas->K2_DrawText(font, classString, { Pos.X, Pos.Y + 15 }, { 153,0 ,0 }, 0, { 255, 0, 0, 255 }, { 0.0 }, 0, 0, true, { 255, 0, 0, 255 });

				}

			}


		}
		//Wywoal orynalny postrender po dodaniu swoich rzeczy
		original_postrender(thisptr, canvas);



	}















	void pointerFuncB(uintptr_t* iptr) {
		/*Print the value pointed to by iptr*/
		printf("Value:  %d\n", *iptr);

		/*Print the address pointed to by iptr*/
		printf("address pointer to:  %p\n", iptr);

		/*Print the address of iptr itself*/
		printf("address of:  %p\n", &iptr);
	}


	bool bartHook()
	{
		
		
		//*vmtPostRender = hooked_postrender;
		std::cout << "b3" << std::endl;
		
		system("pause");
		return true;
	}





	void pointerFuncA(uintptr_t* iptr) {
		/*Print the value pointed to by iptr*/
		printf("Value:  %d\n", *iptr);

		/*Print the address pointed to by iptr*/
		printf("Value:  %p\n", iptr);

		/*Print the address of iptr itself*/
		printf("Value:  %p\n", &iptr);
	}
	void prep()
	{
		CG::InitSdk();
		HookDetour();

	}

	void tick()
	{

		system("cls");
		std::cout << udane << std::endl;
		//GWorldz = *CG::UWorld::GWorld;
		std::cout << "Init succesfully" << std::endl;

		//CG::UWorld* GWorld = GWorld;
		std::cout << "elo2" << std::endl;

		std::cout << &UWorld::GWorld << std::endl;
		std::cout << GWorldz << std::endl;
		if (!GWorldz)
			return;

		//std::cout << "elo4" << std::endl;

		UGameInstance* game_instance = GWorldz->OwningGameInstance;

		if (!game_instance)
		{
			std::cout << "no game instance" << std::endl;

		}
		else {
			std::cout << "Game instance loaded" << std::endl;
		}


		UPlayer* local_uplayer = game_instance->LocalPlayers[0];

		if (!local_uplayer)
		{
			std::cout << "no local uplayer" << std::endl;

		}
		else {
			std::cout << "local uplayerloaded" << std::endl;
		}




		APlayerController* local_controller = game_instance->LocalPlayers[0]->PlayerController;
		if (!local_controller)
		{
			std::cout << "no Local controler" << std::endl;

		}
		else {
			std::cout << "Local controler loaded" << std::endl;
		}

		AActor* local_player = game_instance->LocalPlayers[0]->PlayerController->AcknowledgedPawn;
		if (!local_player)
		{
			std::cout << "no local player" << std::endl;

		}
		else {
			std::cout << "local player loaded" << std::endl;
		}

		ULevel* persistent_level = GWorldz->PersistentLevel;
		if (!persistent_level)
		{
			std::cout << "no persistent_level" << std::endl;

		}
		else {
			std::cout << "persistent_level loaded" << std::endl;
		}


		ULocalPlayer* uLocal_Player = game_instance->LocalPlayers[0];
		UGameViewportClient* viewportClient = uLocal_Player->ViewportClient;


		//pointerFuncA((uintptr_t*)viewportClient);
		//bool espStatus2 = vars::esp;
		//std::cout << "VFTable" << &viewportClient << std::endl;
		if (GetAsyncKeyState(VK_NUMPAD0))
		{
			if (_settings.elo == 1)
			{
				_settings.elo = 0;
			}
			else
			{
				_settings.elo = 1;
			}
		}
			TTransArray<AActor*> actors_array = persistent_level->Actors;


			//UCanvas* canvas


			FVector playerLoc = local_player->K2_GetActorLocation();
			auto _actorLocal = reinterpret_cast<CG::AShooterCharacter*>(local_player);
			std::string eloLocal;
			std::string pname = _actorLocal->AShooterCharacterPlayerName.ToString();
			eloLocal = _actorLocal->GetName();
			std::cout << pname.c_str() << std::endl;
			std::cout << "Player location: " << playerLoc.X << " , " << playerLoc.Y << " , " << playerLoc.Z << std::endl;
			std::cout << actors_array.Count() << " actors active" << std::endl;
			//TTransArray<AActor*> actors_array = persistent_level->Actors;
			for (int i = 50; i < actors_array.Count(); i++)
			{

				AActor* actor = actors_array[i];

				if (!actor)
				{

				}
				else {
					auto _actor = reinterpret_cast<CG::AShooterCharacter*>(actor);
					if (actor->IsShooterCharacter())
					{
						std::string elo;
						elo = _actor->AShooterCharacterPlayerName.ToString();
						//elo = actor->GetDistanceTo(local_player);
						if (elo != "None")
						{

							//std::cout << actor->GetName() << std::endl;
							std::cout << elo << std::endl;
							FVector loc = actor->K2_GetActorLocation();
							std::cout << loc.X << " , " << loc.Y << " , " << loc.Z << std::endl;

						}
					}
				}
			}

			std::string ss;
			std::cout << "Press enter to continue" << std::endl;
			ss = getchar();


		
	}

	bool InitSdk(const std::wstring& moduleName, uintptr_t gObjectsOffset, uintptr_t gNamesOffset, uintptr_t gWorldOffset)
	{
		auto mBaseAddress = reinterpret_cast<uintptr_t>(GetModuleHandleW(moduleName.c_str()));
		if (!mBaseAddress)
			return false;
		
		UObject::GObjects = reinterpret_cast<CG::TUObjectArray*>(mBaseAddress + gObjectsOffset);
		FName::GNames = reinterpret_cast<CG::TNameEntryArray*>(mBaseAddress + gNamesOffset);
		UWorld::GWorld = reinterpret_cast<CG::UWorld**>(mBaseAddress + gWorldOffset);
		









		return true;
	}

	/**
	 * Initialize SDK
	 */
	bool InitSdk()
	{
		return InitSdk(L"ShooterGame.exe", 0x497C9F8, 0x49C7A40, 0x49EB018);
	}

	// --------------------------------------------------
	// # Structs Functions
	// --------------------------------------------------
	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FString.FString
	 * 		Flags  -> ()
	 */
	FString::FString()
	{

	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FString.FString
	 * 		Flags  -> ()
	 * Parameters:
	 * 		const wchar_t*                                     other
	 */
	FString::FString(const wchar_t* other)
	{
		_max = _count = *other ? static_cast<int32_t>(std::wcslen(other)) + 1 : 0;
		if (_count)
			_data = const_cast<wchar_t*>(other);
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FString.wc_str
	 * 		Flags  -> ()
	 */
	const wchar_t* FString::wc_str() const
	{
		return _data;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FString.c_str
	 * 		Flags  -> ()
	 */
	const char* FString::c_str() const
	{
		return (const char*)_data;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FString.IsValid
	 * 		Flags  -> ()
	 */
	bool FString::IsValid() const
	{
		return _data != nullptr;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FString.ToString
	 * 		Flags  -> ()
	 */
	std::string FString::ToString() const
	{
		size_t length = std::wcslen(_data);
		std::string str(length, '\0');
		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(_data, _data + length, '?', &str[0]);
		return str;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FString.ToStringW
	 * 		Flags  -> ()
	 */
	std::wstring FString::ToStringW() const
	{
		std::wstring str(_data);
		return str;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FUObjectItem.IsUnreachable
	 * 		Flags  -> ()
	 */


	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.TUObjectArray.Count
	 * 		Flags  -> ()
	 */
	int32_t TUObjectArray::Count() const
	{
		return NumElements;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.TUObjectArray.Max
	 * 		Flags  -> ()
	 */
	int32_t TUObjectArray::Max() const
	{
		return MaxElements;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.TUObjectArray.IsValidIndex
	 * 		Flags  -> ()
	 * Parameters:
	 * 		int32_t                                            Index
	 */
	bool TUObjectArray::IsValidIndex(int32_t Index) const
	{
		return Index < Count() && Index >= 0;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.TUObjectArray.GetObjectPtr
	 * 		Flags  -> ()
	 * Parameters:
	 * 		int32_t                                            Index
	 */
	FUObjectItem* TUObjectArray::GetObjectPtr(int32_t Index) const
	{
		const int32_t ChunkIndex = Index / NumElementsPerChunk;
		const int32_t WithinChunkIndex = Index % NumElementsPerChunk;
		if (!IsValidIndex(Index)) return nullptr;
		if (ChunkIndex > NumChunks) return nullptr;
		if (Index > MaxElements) return nullptr;
		FUObjectItem* Chunk = Objects[ChunkIndex];
		if (!Chunk) return nullptr;
		return Chunk + WithinChunkIndex;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.TUObjectArray.GetByIndex
	 * 		Flags  -> ()
	 * Parameters:
	 * 		int32_t                                            index
	 */
	UObject* TUObjectArray::GetByIndex(int32_t index) const
	{
		FUObjectItem* ItemPtr = GetObjectPtr(index);
		if (!ItemPtr) return nullptr;
		return (*ItemPtr).Object;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.TUObjectArray.GetItemByIndex
	 * 		Flags  -> ()
	 * Parameters:
	 * 		int32_t                                            index
	 */
	FUObjectItem* TUObjectArray::GetItemByIndex(int32_t index) const
	{
		FUObjectItem* ItemPtr = GetObjectPtr(index);
		if (!ItemPtr) return nullptr;
		return ItemPtr;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.TUObjectArray.operator[]
	 * 		Flags  -> ()
	 * Parameters:
	 * 		int32_t                                            i
	 */
	UObject* TUObjectArray::operator[](int32_t i)
	{
		return GetByIndex(i);
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.TUObjectArray.operator[]
	 * 		Flags  -> ()
	 * Parameters:
	 * 		int32_t                                            i
	 */
	const UObject* TUObjectArray::operator[](int32_t i) const
	{
		return GetByIndex(i);
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FNameEntry.GetIndex
	 * 		Flags  -> ()
	 */
	const int32_t FNameEntry::GetIndex() const
	{
		return Index >> NAME_INDEX_SHIFT;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FNameEntry.IsWide
	 * 		Flags  -> ()
	 */
	bool FNameEntry::IsWide() const
	{
		return Index & NAME_WIDE_MASK;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FNameEntry.GetAnsiName
	 * 		Flags  -> ()
	 */
	std::string FNameEntry::GetAnsiName() const
	{
		return std::string(AnsiName);
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FNameEntry.GetWideName
	 * 		Flags  -> ()
	 */
	std::wstring FNameEntry::GetWideName() const
	{
		return std::wstring(WideName);
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FNameEntry.GetName
	 * 		Flags  -> ()
	 */
	std::string FNameEntry::GetName() const
	{
		return GetAnsiName();
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FName.FName
	 * 		Flags  -> ()
	 */
	FName::FName()
	{
		ComparisonIndex = 0;
		Number = 0;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FName.FName
	 * 		Flags  -> ()
	 * Parameters:
	 * 		int32_t                                            i
	 */
	FName::FName(int32_t i)
	{
		ComparisonIndex = i;
		Number = 0;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FName.FName
	 * 		Flags  -> ()
	 * Parameters:
	 * 		const char*                                        nameToFind
	 */
	FName::FName(const char* nameToFind)
	{
		Number = 0;
		static std::unordered_set<int> cache;
		for (auto i : cache)
		{
			if (GetGlobalNames()[i]->GetAnsiName() == nameToFind)
			{
				ComparisonIndex = i;
				#ifdef FNAME_POOL_WITH_CASE_PRESERVING_NAME
				DisplayIndex = i;
				#endif
				return;
			}
		}
		
		#ifdef FNAME_POOL
		uintptr_t lastFNameAddress = NULL;
		for (FNameEntry* name = GetGlobalNames().GetNext(lastFNameAddress); name != nullptr; name = GetGlobalNames().GetNext(lastFNameAddress))
		{
			if (name->GetAnsiName() == nameToFind)
			{
				cache.insert(name->GetId());
				ComparisonIndex = name->GetId();
				#ifdef FNAME_POOL_WITH_CASE_PRESERVING_NAME
				DisplayIndex = name->GetId();
				#endif
				return;
			}
		}
		#else
		for (int32_t i = 0; i < GetGlobalNames().Count(); ++i)
		{
			if (GetGlobalNames()[i]->GetAnsiName() == nameToFind)
			{
				cache.insert(i);
				ComparisonIndex = i;
				return;
			}
		}
		#endif
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FName.FName
	 * 		Flags  -> ()
	 * Parameters:
	 * 		const wchar_t*                                     nameToFind
	 */
	FName::FName(const wchar_t* nameToFind)
	{
		Number = 0;
		static std::unordered_set<int> cache;
		for (auto i : cache)
		{
			if (GetGlobalNames()[i]->GetWideName() == nameToFind)
			{
				ComparisonIndex = i;
				#ifdef FNAME_POOL_WITH_CASE_PRESERVING_NAME
				DisplayIndex = i;
				#endif
				return;
			}
		}
		
		#ifdef FNAME_POOL
		uintptr_t lastFNameAddress = NULL;
		for (FNameEntry* name = GetGlobalNames().GetNext(lastFNameAddress); name != nullptr; name = GetGlobalNames().GetNext(lastFNameAddress))
		{
			if (name->GetWideName() == nameToFind)
			{
				cache.insert(name->GetId());
				ComparisonIndex = name->GetId();
				#ifdef FNAME_POOL_WITH_CASE_PRESERVING_NAME
				DisplayIndex = name->GetId();
				#endif
				return;
			}
		}
		#else
		for (int32_t i = 0; i < GetGlobalNames().Count(); ++i)
		{
			if (GetGlobalNames()[i]->GetWideName() == nameToFind)
			{
				cache.insert(i);
				ComparisonIndex = i;
				return;
			}
		}
		#endif
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FName.GetGlobalNames
	 * 		Flags  -> ()
	 */
	TNameEntryArray& FName::GetGlobalNames()
	{
		return *GNames;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FName.GetNameA
	 * 		Flags  -> ()
	 */
	std::string FName::GetNameA() const
	{
		return GetGlobalNames()[ComparisonIndex]->GetAnsiName();
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FName.GetNameW
	 * 		Flags  -> ()
	 */
	std::wstring FName::GetNameW() const
	{
		return GetGlobalNames()[ComparisonIndex]->GetWideName();
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FName.GetName
	 * 		Flags  -> ()
	 */
	std::string FName::GetName() const
	{
		return GetNameA();
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FScriptInterface.GetObjectPtr
	 * 		Flags  -> ()
	 */
	UObject* FScriptInterface::GetObjectPtr() const
	{
		return ObjectPointer;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FScriptInterface.GetObjectRef
	 * 		Flags  -> ()
	 */
	UObject*& FScriptInterface::GetObjectRef()
	{
		return ObjectPointer;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FScriptInterface.GetInterface
	 * 		Flags  -> ()
	 */
	void* FScriptInterface::GetInterface() const
	{
		return ObjectPointer != nullptr ? InterfacePointer : nullptr;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FText.Get
	 * 		Flags  -> ()
	 */
	wchar_t* FText::Get() const
	{
		return Data != nullptr ? Data->Name : nullptr;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FText.ToString
	 * 		Flags  -> ()
	 */
	std::string FText::ToString() const
	{
		wchar_t* name = Get();
		if (!name)
		    return "NOT FOUND";
		
		size_t length = std::wcslen(name);
		std::string str(length, '\0');
		std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(name, name + length, '?', &str[0]);
		
		return str;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FText.ToWString
	 * 		Flags  -> ()
	 */
	std::wstring FText::ToWString() const
	{
		wchar_t* name = Get();
		if (!name)
		    return L"NOT FOUND";
		
		std::wstring str(name);
		return str;
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FWeakObjectPtr.SerialNumbersMatch
	 * 		Flags  -> ()
	 * Parameters:
	 * 		FUObjectItem*                                      objectItem
	 */
	bool FWeakObjectPtr::SerialNumbersMatch(FUObjectItem* objectItem) const
	{
		return objectItem->SerialNumber == ObjectSerialNumber;
	}

	bool FUObjectItem::IsUnreachable() const
	{
		return !!(static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::Unreachable));
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FUObjectItem.IsPendingKill
	 * 		Flags  -> ()
	 */
	bool FUObjectItem::IsPendingKill() const
	{
		return !!(static_cast<std::underlying_type_t<ObjectFlags>>(ObjectFlags::PendingKill));
	}



	bool FWeakObjectPtr::IsValid() const
	{
		if (ObjectSerialNumber == 0 || ObjectIndex < 0)
			return false;

		auto ObjectItem = UObject::GetGlobalObjects().GetItemByIndex(ObjectIndex);
		if (!ObjectItem)
			return false;

		if (!SerialNumbersMatch(ObjectItem))
			return false;

		return !(ObjectItem->IsUnreachable() || ObjectItem->IsPendingKill());
	}

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FWeakObjectPtr.IsValid
	 * 		Flags  -> ()
	 */
	

	/**
	 * Function:
	 * 		RVA    -> 0x00000000
	 * 		Name   -> PredefinedFunction BasicTypes.FWeakObjectPtr.Get
	 * 		Flags  -> ()
	 */
	UObject* FWeakObjectPtr::Get() const
	{
		if (!IsValid())
			return nullptr;
		
		auto ObjectItem = UObject::GetGlobalObjects().GetItemByIndex(ObjectIndex);
		if (!ObjectItem)
			return nullptr;
		
		return ObjectItem->Object;
	}

}


