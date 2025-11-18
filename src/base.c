#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0502
#define STRICT
#include <windows.h>
#include <mmsystem.h>

#include "res.h"
#include "base.h"

HINSTANCE vhInst;

void Entry(void);

HWND CreateAppWindow(void);
const char *EdgeLight = "EdgeLight";

#ifndef RELEASE
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR pszCmdLine, int nCmdShow)
{
  Entry();
  return 0;
}
#endif

void Entry(void)
{
	MSG msg;
	HWND hwnd;

	vhInst = GetModuleHandle(NULL);

	if(!InitApplication()){
		msg.wParam = 1;
		goto EXIT;
	}
	hwnd = CreateAppWindow();
	if (!hwnd) {
		msg.wParam = 1;
		goto EXIT;
	}

	ShowWindow(hwnd, SW_SHOW);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}


EXIT:
	ExitProcess(msg.wParam);
}


HWND CreateAppWindow(void)
{
  HWND hwnd;

  hwnd = CreateWindowEx(
    WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
    WindowClass, EdgeLight,
    WS_POPUP,
    CW_USEDEFAULT, CW_USEDEFAULT,
    0, 0,
    NULL, NULL, vhInst, NULL);

  return hwnd;
}


int InitApplication(void)
{
  WNDCLASS wc;

  /* for hidden window */
  wc.style         = CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW|CS_BYTEALIGNCLIENT;
  wc.lpfnWndProc   = (WNDPROC)WndProc;
  wc.cbClsExtra    = 0;
  wc.cbWndExtra    = 0;
  wc.hInstance     = vhInst;
  wc.hIcon         = LoadIcon(vhInst, MAKEINTRESOURCE(IDI_ICON));
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(255, 255, 255));
  wc.lpszMenuName  = NULL;
  wc.lpszClassName = WindowClass;

  if(!RegisterClass(&wc)){
    return 0;
  }

  return 1;
}

