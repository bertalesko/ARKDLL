#pragma once
#include "pch.h"
#include "console.h"
#include "includes.h"



#define show_console 1 //1 = show console ~ 0 = don't show console


DWORD WINAPI MainThread(LPVOID param) {
   
    
    __try
    {
        CG::prep();

    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}

   
    while (true) {
       


        __try
        {
            CG::tick();

        }
        __except (EXCEPTION_EXECUTE_HANDLER) {}



       
        if (GetAsyncKeyState(VK_INSERT) & 1) { //if Insert is pressed make a short beep and free the console (if used)
           

            Beep(300, 100); //just so you know it registered the keypress; you can remove it if want
         

            if (show_console) {

                auto temp = GetConsoleWindow();
                FreeConsole();
                PostMessage(temp, WM_QUIT, 0, 0);

            }

            break;
        }

        Sleep(1);
    }

    FreeLibraryAndExitThread((HMODULE)param, 0); //erease traces of your dll (!)
    return 0;
}



BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        if (show_console) console(hModule); //opens console if show_console is set to 1 (how? -> console.h)
    }

    case DLL_PROCESS_DETACH: {

        break;
    }
    }

    return TRUE;
}