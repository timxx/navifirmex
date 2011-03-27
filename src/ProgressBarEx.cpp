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

#include "ProgressBarEx.h"
#include <strsafe.h>

ProgressBarEx::ProgressBarEx(void)
{
	_lpfnOldProc = NULL;
	SetFont(12, TEXT("宋体"));
}

ProgressBarEx::~ProgressBarEx(void)
{
	if (_lpfnOldProc)
		Subclass(_lpfnOldProc);
}

LRESULT CALLBACK ProgressBarEx::ProgressProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ProgressBarEx *pCtl = (ProgressBarEx*)::GetWindowLongPtr(hWnd, GWL_USERDATA);
	static Rect rect;

	switch(uMsg)
	{
	case WM_PAINT:
		{
			//不能用BeginPaint...
			HDC hdc = GetDC(hWnd);
			
			CallWindowProc(pCtl->_lpfnOldProc, hWnd, uMsg, wParam, lParam);

			SetTextColor(hdc, /*RGB(63, 72, 204)*/pCtl->_crText);
			SetBkMode(hdc, TRANSPARENT);
			HGDIOBJ hOldObj = SelectObject(hdc, pCtl->_hFont);
			::GetClientRect(hWnd, &rect);

			int nCurPos = pCtl->GetPos();
			PBRANGE pbRange;
			int nMax = pCtl->GetRange(TRUE, &pbRange);
			TCHAR szPercent[128];
			if (pbRange.iHigh == 0)
				pbRange.iHigh = (pbRange.iLow == 0 ? 1 : pbRange.iLow);
			StringCchPrintf(szPercent, 128, TEXT("%.2f%%"), (double)(nCurPos - pbRange.iLow)*100.0/pbRange.iHigh);
			DrawText(hdc, szPercent, lstrlen(szPercent), &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			SelectObject(hdc, hOldObj);
			ReleaseDC(hWnd, hdc);
		}
		break;
	}

	return CallWindowProc(pCtl->_lpfnOldProc, hWnd, uMsg, wParam, lParam);
}