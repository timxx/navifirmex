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
#pragma once

#ifndef __PROGRESS_BAR_EX_H__
#define __PROGRESS_BAR_EX_H__

#include "tim\commonctrls.h"

using namespace _TIM;

class ProgressBarEx : public ProgressCtl
{
public:
	ProgressBarEx();
	~ProgressBarEx();

	virtual void assign(UINT uId, HWND hwndParent /* = NULL */)
	{
		ProgressCtl::assign(uId, hwndParent);

		_lpfnOldProc = Subclass(ProgressProc);
	}

	virtual HWND create(LPCTSTR lpTitle = TEXT("Progress Bar"), HWND hwndParent = NULL,
		int id = -1, int nWidth = 0, int nHeight = 0,
		int x =0 , int y = 0, DWORD dwStyle = PBS_SMOOTH)
	{
		_hWnd = ProgressCtl::create(lpTitle, hwndParent, id, nWidth, nHeight, x, y, dwStyle);
		_lpfnOldProc = Subclass(ProgressProc);
		return _hWnd;
	}

	void SetFont(DWORD dwSize, const TString &fontName, COLORREF crText = RGB(0, 0, 0))
	{
		if (_hFont)	{
			DeleteObject(_hFont);
		}

		_crText = crText;
		_hFont = CreateFont(dwSize, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, fontName);
	}
protected:
	static LRESULT CALLBACK ProgressProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	WNDPROC	_lpfnOldProc;
	HFONT	_hFont;
	COLORREF _crText;
};

#endif