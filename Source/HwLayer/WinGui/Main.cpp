#include <Source/Main/Application.h>

__declspec(dllimport) int Main(CApplicationProto* pApp);

#ifdef _CONSOLE
int main(void)
#else
int APIENTRY WinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
#endif
{ 
	CApplication app;
	return Main(&app);
}