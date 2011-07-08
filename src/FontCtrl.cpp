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

#include "FontCtrl.h"

using namespace Tim;

FontCtrl::FontCtrl(void)
{
	_color = RGB(0, 0, 0);
	RtlSecureZeroMemory(&_lFont, sizeof(LOGFONT));

	_tcscpy_s(_lFont.lfFaceName, 32, TEXT("ËÎÌו"));
	_lFont.lfHeight = 12;

	_font = CreateFontIndirect(&_lFont);

	_hfUnderlined = NULL;
	_fHovered = false;
}

FontCtrl::~FontCtrl(void)
{
	if (_font)
		DeleteObject(_font);
	if (_hfUnderlined)
		DeleteObject(_hfUnderlined);
}

LRESULT CALLBACK FontCtrl::SubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	FontCtrl *pCtl = (FontCtrl*)::GetWindowLongPtr(hWnd, GWL_USERDATA);

	return pCtl->runProc(hWnd, uMsg, wParam, lParam);
}

LRESULT FontCtrl::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		{
			Rect rect;
			GetClientRect(&rect);

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(_hWnd, &ps);

			SetTextColor(hdc, _color);

			SetBkMode(hdc, TRANSPARENT);

			HGDIOBJ hOldObj = SelectObject(hdc, _fHovered ? _hfUnderlined : _font);

			TString str = getText();
			DrawText(hdc, str, str.length(), &rect, DT_SINGLELINE | DT_CENTER);

			SelectObject(hdc, hOldObj);

			EndPaint(_hWnd, &ps);
		}
		return 0;

	case WM_MOUSEMOVE:
		if (!_fHovered)
		{
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = hWnd;
			tme.dwFlags = TME_LEAVE | TME_HOVER;
			tme.dwHoverTime = 5;
			_fHovered = _TrackMouseEvent(&tme);
		}

		return 0;

	case WM_MOUSEHOVER:
		if (_hfUnderlined == NULL && _font != NULL)
		{
			if (!_lFont.lfUnderline)
			{
				_lFont.lfUnderline = TRUE;
				_hfUnderlined = ::CreateFontIndirect(&_lFont);
				_lFont.lfUnderline = FALSE;
			}
			else
				_hfUnderlined = ::CreateFontIndirect(&_lFont);
		}

		SetClassLong(hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_HAND));

		update();
		return 0;

	case WM_MOUSELEAVE:
		SetClassLong(hWnd, GCL_HCURSOR, (LONG)LoadCursor(NULL, IDC_ARROW));
		_fHovered = false;
		update();
		return 0;

	case WM_NCHITTEST:
		return HTCLIENT;

	case WM_LBUTTONUP:
		chooseFont();
		::SendMessage(getParent(), WM_COMMAND, MAKEWPARAM(_id, 0), NULL);
		return 0;
	}

	return ::CallWindowProc(_lpfnOld, hWnd, uMsg, wParam, lParam);
}

void FontCtrl::update()
{
	Rect rect;
	GetWindowRect(&rect);

	::ScreenToClient(getParent(), (LPPOINT)&rect);			// left top
	::ScreenToClient(getParent(), (LPPOINT)(&rect.right));	// right down

	::InvalidateRect(getParent(), &rect, TRUE);
}

void FontCtrl::chooseFont()
{
	CHOOSEFONT cf = {0};

	cf.lStructSize	= sizeof(cf);
	cf.hwndOwner	= _hWnd;
	cf.lpLogFont	= &_lFont;
	cf.rgbColors	= _color;
	cf.Flags		= CF_SCREENFONTS | CF_EFFECTS | CF_LIMITSIZE | CF_INITTOLOGFONTSTRUCT;
	cf.nSizeMin		= 8;
	cf.nSizeMax		= 16;

	if (ChooseFont(&cf))
	{
		_color = cf.rgbColors;

		if (_font)
			DeleteObject(_font);

		if (_hfUnderlined)
		{
			DeleteObject(_hfUnderlined);
			_hfUnderlined = NULL;
		}

		_font = CreateFontIndirect(&_lFont);

		update();
	}
}