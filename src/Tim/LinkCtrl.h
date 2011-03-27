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
//////////////////////////////////////////////////////////////////////////
//
//Modified by Just Fancy, (original from Notepad++ source)
//
//////////////////////////////////////////////////////////////////////////
#ifndef URLCTRL_INCLUDED
#define URLCTRL_INCLUDED
//========================================================================================================
#pragma once
//========================================================================================================
#include "Window.h"
//========================================================================================================
_TIM_BEGIN

class LinkCtrl : public Window
{
public:
    LinkCtrl():_hfUnderlined(0),_hCursor(0), _msgDest(NULL), _cmdID(0), _oldproc(NULL), \
		_linkColor(), _visitedColor(), _clicking(false), _URL(_T("")){};

    void create(HWND itemHandle, TCHAR * link, COLORREF linkColor = RGB(0,0,255));
	void create(HWND itemHandle, int cmd, HWND msgDest = NULL);
    void destroy();

protected :
    TString _URL;
    HFONT	_hfUnderlined;
    HCURSOR	_hCursor;

	HWND _msgDest;
	unsigned long _cmdID;

    WNDPROC  _oldproc;
    COLORREF _linkColor;			
    COLORREF _visitedColor;
    bool  _clicking;

    static LRESULT CALLBACK LinkCtrlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
        return ((LinkCtrl *)(::GetWindowLongPtr(hWnd, GWL_USERDATA)))->runProc(hWnd, uMsg, wParam, lParam);
    };
    LRESULT runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

_TIM_END

#endif //URLCTRL_INCLUDED
