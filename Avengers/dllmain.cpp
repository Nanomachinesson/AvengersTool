// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Avengers.h"
#include <thread>

int main()
{
    Avengers instHud;
    //wait for the flag for exit to be set
    while (!instHud.exit)
        Sleep(500);
    //when this function returns the destructor of main will be called and all of the singletons inside it will also be destroyed and deconstructed
    return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ulReasonForCall,
                       LPVOID lpReserved
                     )
{
    switch (ulReasonForCall)
    {
    case DLL_PROCESS_ATTACH:
    {
        //create a thread and detach it so anything we do inside of main is non-blocking for the loading thread
        std::thread mainThread(main);
        mainThread.detach();
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        Avengers::getInstance()->exit = true;
        break;
    }
    return TRUE;
}

