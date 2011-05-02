

#include "StatusEx.h"
#include "res\resource.h"

using namespace Tim;

void StatusEx::init(HINSTANCE hinst, HWND hwndParent)
{
	Window::init(hinst, hwndParent);

	_hbmpProcess = LoadBitmap(hinst, MAKEINTRESOURCE(IDB_PROCESS));
}

LRESULT CALLBACK StatusEx::SubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	StatusEx *pCtl = (StatusEx*)::GetWindowLongPtr(hWnd, GWL_USERDATA);

	return pCtl->runProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK StatusEx::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
		if (_hbmpProcess)
		{
			HDC hdc = GetDC(hWnd);
			HDC hdcMem = CreateCompatibleDC(hdc);
			HGDIOBJ hOldObj = SelectObject(hdcMem, _hbmpProcess);

			Rect rect;
			GetRect(0, &rect);

			int x = rect.left + (rect.Width() - 16)/2;
			int y = rect.top + (rect.Height() - 16)/2;

			TransparentBlt(hdc, x, y, 16, 16, hdcMem, _processIndex * 16, 0, 16, 16, RGB(255, 255, 255));

			SelectObject(hdcMem, hOldObj);
			DeleteDC(hdcMem);
			ReleaseDC(_hWnd, hdc);
		}

		break;

	case WM_TIMER:
		{
			if (_processIndex++ > 30)
				_processIndex = 0;

			Rect rect;
			GetRect(0, &rect);

			InvalidateRect(&rect);
		}
		break;
	}

	return CallWindowProc(_lpfnOldProc, hWnd, uMsg, wParam, lParam);
}