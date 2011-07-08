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

#ifndef __FONT_CTRL_H__
#define __FONT_CTRL_H__

#include "Tim\CommonCtrls.h"

class FontCtrl : public Tim::Static
{
public:
	FontCtrl();
	~FontCtrl();

	virtual void assign(HWND hwndStatic, HWND hwndParent = NULL)
	{
		Static::assign(hwndStatic, hwndParent);
		setStyle(getStyle() | SS_NOTIFY);
		_lpfnOld = Subclass(SubProc);
	}

	COLORREF getColor() const			{	return _color;		}
	const LOGFONT& getFont() const		{	return _lFont;		}

	void setFont(const LOGFONT &lf, COLORREF cr)
	{
		_lFont = lf;
		_color = cr;
		update();
	}

private:
	void update();

	void chooseFont();

protected:
	static LRESULT CALLBACK SubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	COLORREF	 _color;	// font color

	LOGFONT		_lFont;

	HFONT		_font;
	HFONT		_hfUnderlined;

	WNDPROC  _lpfnOld;

	bool	_fHovered;
};

#endif // __FONT_CTRL_H__