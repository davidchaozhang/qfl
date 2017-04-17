
#include <io.h>
#include <fcntl.h>
#include <afxmt.h>


void useConsoleWindow()
{
	/* open console */
	FreeConsole();
	AllocConsole();
	int hCrt = _open_osfhandle((intptr_t) GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
	FILE *hf = _fdopen(hCrt, "w");
	*stdout = *hf;
	hCrt = _open_osfhandle((intptr_t) GetStdHandle(STD_INPUT_HANDLE), _O_TEXT);
	hf = _fdopen(hCrt, "r");
	*stdin = *hf;
	hCrt = _open_osfhandle((intptr_t) GetStdHandle(STD_ERROR_HANDLE), _O_TEXT);
	hf = _fdopen(hCrt, "w");
	*stderr = *hf;
	setvbuf(stderr, NULL, _IONBF, 0);

	wchar_t  buf[100];
	wsprintf(buf, _T("Debug Window"), _T("Then disable menu"));
	SetConsoleTitle((LPCTSTR) buf);

	HWND hwnd = NULL;
	while(hwnd == NULL) {
		hwnd = ::FindWindowEx(NULL, NULL, NULL, (LPCTSTR)buf);
	}

	LPCTSTR cTitle = L"Win32 Debug Message Window";
	SetConsoleTitle(cTitle);

	HMENU hmenu = GetSystemMenu(hwnd, FALSE);
	DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND);
};