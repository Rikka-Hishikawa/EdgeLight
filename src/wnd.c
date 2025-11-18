#define WIN32_LEAN_AND_MEAN
#define STRICT
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <commctrl.h>

#include "base.h"
#include "res.h"

#define TRAYMESSAGE (WM_APP + 1)

static NOTIFYICONDATA ni = {
  sizeof(NOTIFYICONDATA),
  NULL,
  1,
  NIF_MESSAGE | NIF_ICON | NIF_TIP,
  TRAYMESSAGE,
  NULL,
  "EdgeLight"
};

int vOpaque = 128;
static void OnCreate(HWND hwnd);
static void OnDestroy(HWND hwnd);
static void OnPaint(HWND hwnd);
static void MakeRegion(HWND hwnd, int cx, int cy, SIZE *sz);
int CALLBACK OptProc(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam);
void OnTrayMessage(HWND hwnd, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch(uMsg){
  case WM_CREATE:
    OnCreate(hwnd);
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_PAINT:
    OnPaint(hwnd);
    break;
  case TRAYMESSAGE:
    OnTrayMessage(hwnd, wParam, lParam);
    break;
  case WM_DISPLAYCHANGE:
    OnCreate(hwnd);
    break;

  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
  return 0L;
}

static void OnCreate(HWND hwnd)
{
  SIZE sz;
  int cx, cy;

  cx = GetSystemMetrics(SM_CXSCREEN);
  cy = GetSystemMetrics(SM_CYSCREEN);

  MakeRegion(hwnd, cx, cy, &sz);


  SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, cx, cy, SWP_NOACTIVATE);
  SetLayeredWindowAttributes(hwnd, 0, vOpaque, LWA_ALPHA);

  ni.hWnd = hwnd;
  ni.hIcon = (HICON)LoadIcon(vhInst, MAKEINTRESOURCE(IDI_ICON));
  Shell_NotifyIcon(NIM_ADD, &ni);
}


static void OnPaint(HWND hwnd)
{
  HDC dc;
  PAINTSTRUCT ps;

  dc = BeginPaint(hwnd, &ps);
  EndPaint(hwnd, &ps);
}


static void MakeRegion(HWND hwnd, int cx, int cy, SIZE *sz)
{
	HRGN rgn_full;
	HRGN rgn_clip;
	int sx;
	int sy;

	sx = cx / 24;
	sy = cy / 24;

	if (sx < sy) {
		sx = sy;
	} else {
		sy = sx;
	}

	rgn_full = CreateRectRgn(0, 0, cx, cy);
	rgn_clip = CreateRectRgn(sx, sx, cx - sx, cy - sy);
	CombineRgn(rgn_full, rgn_full, rgn_clip, RGN_XOR);
	DeleteObject(rgn_clip);

	SetWindowRgn(hwnd, rgn_full, TRUE);
}

void init_optdialog(HWND hwnd, LPARAM lparam)
{
	POINT *pt;
	RECT rc;
	HWND h;

	InitCommonControls();
	pt = (POINT*)lparam;

	GetWindowRect(hwnd, &rc);

	SetWindowPos(hwnd, NULL, pt->x, pt->y - rc.bottom, 0, 0, SWP_NOSIZE);
	h = GetDlgItem(hwnd, IDC_OPAQUE);
	SendMessage(h, TBM_SETRANGE, TRUE, MAKELPARAM(0, 255));
	SendMessage(h, TBM_SETTICFREQ, 16, 0);
	SendMessage(h, TBM_SETPOS, 1, vOpaque);

	SetFocus(h);
}

int CALLBACK OptProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	int pos;
	HWND parent;
	
	switch (umsg) {
	case WM_INITDIALOG:
		init_optdialog(hwnd, lparam);
		break;
	case WM_ACTIVATE:
		if (wparam == WA_INACTIVE) {
			EndDialog(hwnd, 0);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			EndDialog(hwnd, 0);
			return TRUE;
			break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			return TRUE;
			break;
		}
		break;
	case WM_VSCROLL:
		pos = SendDlgItemMessage(hwnd, IDC_OPAQUE, TBM_GETPOS, 0, 0);
		parent = GetParent(hwnd);
		SetLayeredWindowAttributes(parent, 0, 255 - pos, LWA_ALPHA);
		vOpaque = pos;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

void OnTrayMessage(HWND hwnd, WPARAM wParam, LPARAM lParam)
{
  POINT pt;

	if(wParam != 1) {
		return;
	}
	
	switch (lParam) {
	case WM_LBUTTONDOWN:
		SetForegroundWindow(hwnd);
		GetCursorPos(&pt);
		DialogBoxParam(vhInst, MAKEINTRESOURCE(IDD_OPTION), hwnd, (DLGPROC)OptProc, (LPARAM)&pt);
		break;
	case WM_RBUTTONDOWN:
		DestroyWindow(hwnd);
		break;
	}
}


