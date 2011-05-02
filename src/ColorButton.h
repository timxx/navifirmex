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

#ifndef __COLOR_BUTTON_H__
#define __COLOR_BUTTON_H__

#include "Tim\commonctrls.h"

class ColorButton : public Tim::Button
{
public:
	ColorButton()
		:_lpfnOldProc(0), _color(RGB(255, 255, 255)),
		_crFrame(0), _fShowFrame(true)
	{}

	~ColorButton()
	{
		Subclass(_lpfnOldProc);
	}

	virtual void assign(UINT uId, HWND hwndParent /* = NULL */)
	{
		ControlBase::assign(uId, hwndParent);

		_lpfnOldProc = Subclass(ButtonProc);
	}

	void setColor(COLORREF cr)
	{
		_color = cr;
		update();
	}

	void setFrameColor(COLORREF cr)
	{
		_crFrame = cr;
		update();
	}

	void showFrame(bool fShow = true)
	{
		_fShowFrame = fShow;
		update();
	}

protected:
	static LRESULT CALLBACK ButtonProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void draw(HDC hdc);
	void update();

protected:
	WNDPROC _lpfnOldProc;

	COLORREF _color;
	COLORREF _crFrame;

	bool _fShowFrame;
};

#endif //__COLOR_BUTTON_H__