#include "core/mainframe.h"
#include "stdafx.h"

#if defined(WISPY_WINDOWS)
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                   _In_ char *pCmdLine, _In_ int nCmdShow)
#else
int main(int argc, const char **argv)
#endif
{
#if defined(_DEBUG) && defined(WISPY_WINDOWS)
  _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
  _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

  if (!AllocConsole()) {
    return EXIT_FAILURE;
  }

  FILE *fDummy;
  freopen_s(&fDummy, "CONOUT$", "w", stdout);
  freopen_s(&fDummy, "CONOUT$", "w", stderr);
  freopen_s(&fDummy, "CONIN$", "r", stdin);

  fflush(stdout);
  fflush(stderr);
  fflush(stdin);
#endif

  w_state *state = init_mainframe();
#if defined(WISPY_WINDOWS)
  state->hInstance = hInstance;
#endif

  if (!state)
    return EXIT_FAILURE;

  loop_mainframe(state);
  destroy_mainframe(state);

#if defined(_DEBUG) && defined(WISPY_WINDOWS)
  FreeConsole();
  _CrtDumpMemoryLeaks();
#endif

  return EXIT_SUCCESS;
}
