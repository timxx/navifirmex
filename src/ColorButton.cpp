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

#include "ColorButton.h"

using namespace Tim;

LRESULT CALLBACK ColorButton::ButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ColorButton *pCtl = (ColorButton*)::GetWindowLongPtr(hWnd, GWL_USERDATA);

	return pCtl->runProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ColorButton::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(_hWnd, &ps);

			draw(hdc);

			EndPaint(_hWnd, &ps);
		}
		return 0;
	}

	return CallWindowProc(_lpfnOldProc, hWnd, uMsg, wParam, lParam);
}

void ColorButton::draw(HDC hdc)
{
	Rect rect;
	GetClientRect(&rect);

	HBRUSH hbr = CreateSolidBrush(_color);

	FillRect(hdc, &rect, hbr);

	DeleteObject(hbr);

	if (_fShowFrame)
	{
		HPEN hPen = CreatePen(PS_SOLID, 1, _crFrame);
		HGDIOBJ hOldObj = SelectObject(hdc, hPen);

		MoveToEx(hdc, rect.left, rect.top, NULL);
		LineTo(hdc, rect.right, rect.top);

		LineTo(hdc, rect.right, rect.bottom);

		LineTo(hdc, rect.left, rect.bottom);

		LineTo(hdc, rect.left, rect.top);

		SelectObject(hdc, hOldObj);
		DeleteObject(hPen);
	}
}

void ColorButton::update()
{
	Rect rect;
	GetClientRect(&rect);

	InvalidateRect(&rect, TRUE);
}