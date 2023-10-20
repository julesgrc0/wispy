#include "stdafx.h"
#include "mainframe.h"

#ifdef _WIN32 
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ int nCmdShow)
#else
int main(int argc, const char** argv)
#endif
{
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

#ifdef _DEBUG && _WIN32
    if (AllocConsole())
    {
        FILE* fDummy;
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONIN$", "r", stdin);

        fflush(stdout);
        fflush(stderr);
        fflush(stdin);
    }
#endif // _DEBUG && _WIN32


    State* state = init_mainframe();
    if (!state) return EXIT_FAILURE;

#ifdef _WIN32
    state->hInstance = hInstance;
#endif // _WIN32


    loop_mainframe(state);
    destroy_mainframe(state);

    _CrtDumpMemoryLeaks();
    return EXIT_SUCCESS;
}