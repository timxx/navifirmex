/*
http://code.google.com/p/navifirmex/
Copyright (C) 2011  Just Fancy (Just_Fancy@live.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//========================================================================================================
#include "LinkCtrl.h"
//========================================================================================================
using namespace _TIM;
//========================================================================================================
LRESULT CALLBACK LinkCtrl::LinkCtrlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LinkCtrl *pCtl = (LinkCtrl*)::GetWindowLongPtr(hWnd, GWL_USERDATA);

	return pCtl->runProc(hWnd, uMsg, wParam, lParam);
}

void LinkCtrl::create(HWND hwndStatic, TCHAR *link, COLORREF linkColor)
{
	_hWnd = hwndStatic;

	setStyle(getStyle() | SS_NOTIFY);

	if (link)
		_url = link;

	_linkColor = linkColor;

	_lpfnOld = Subclass(LinkCtrlProc);

	setCursor(hwndStatic);
}

void LinkCtrl::create(HWND hwndStatic, int cmd, HWND msgDest)
{
	_hWnd = hwndStatic;

	setStyle(getStyle() | SS_NOTIFY);

	_cmdID = cmd;
	_msgDest = msgDest;

	_lpfnOld = Subclass(LinkCtrlProc);

	setCursor(hwndStatic);
}

LRESULT LinkCtrl::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch(uMsg)
	{
	case WM_PAINT:
		OnPaint();
		return 0;

	case WM_SETTEXT:
		{
			LRESULT ret = ::CallWindowProc(_lpfnOld, hWnd, uMsg, wParam, lParam);
			InvalidateRect();
			return ret;
		}

	case WM_LBUTTONDOWN:
		return 0;

	case WM_LBUTTONUP:
		if (_cmdID)
		{
			::SendMessage(_msgDest?_msgDest:_parentWnd, WM_COMMAND, _cmdID, 0);
		}
		else
		{
			_linkColor = _visitedColor;

			InvalidateRect();
			UpdateWindow();

			if(!_url.empty()){
				ShellExecute(NULL, TEXT("open"), _url.c_str(), NULL, NULL, SW_SHOWNORMAL);
			}else{
				ShellExecute(NULL, TEXT("open"), getText(), NULL, NULL, SW_SHOWNORMAL);
			}
		}
		break;

	// A standard static control returns HTTRANSPARENT here, which
	// prevents us from receiving any mouse messages. So, return
	// HTCLIENT instead.
	case WM_NCHITTEST:
		return HTCLIENT;
	}

	return ::CallWindowProc(_lpfnOld, hWnd, uMsg, wParam, lParam);
}

void LinkCtrl::setCursor(HWND hWnd)
{
	if (_hCursor == 0)
		_hCursor = LoadCursor(NULL, IDC_HAND);

	SetClassLong(hWnd, GCL_HCURSOR, (LONG)_hCursor);
}

void LinkCtrl::OnPaint()
{
	DWORD dwStyle = getStyle();
	DWORD dwDTStyle = DT_SINGLELINE;

	//Test if centered horizontally or vertically
	if(dwStyle & SS_CENTER)	     dwDTStyle |= DT_CENTER;
	if(dwStyle & SS_RIGHT)		 dwDTStyle |= DT_RIGHT;
	if(dwStyle & SS_CENTERIMAGE) dwDTStyle |= DT_VCENTER;

	Rect rect;
	GetClientRect(&rect);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(_hWnd, &ps);

	//
	SetTextColor(hdc, _linkColor);

	//Background should be transparent
	SetBkMode(hdc, TRANSPARENT);

	// Create an underline font 
	if(_hfUnderlined == NULL)
	{
		// Get the default GUI font
		HFONT hf = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);

		// Add UNDERLINE attribute
		LOGFONT lf;
		GetObject(hf, sizeof lf, &lf);
		lf.lfUnderline = TRUE;

		// Create a new font
		_hfUnderlined = ::CreateFontIndirect(&lf);
	}

	HGDIOBJ hOldObj = SelectObject(hdc, _hfUnderlined);

	TString str = getText();
	DrawText(hdc, str, str.length(), &rect, dwDTStyle);

	SelectObject(hdc, hOldObj);

	EndPaint(_hWnd, &ps);
}