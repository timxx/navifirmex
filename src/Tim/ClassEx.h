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
//================================================================
#pragma once

#ifndef CLASSEX_H
#define CLASSEX_H

#include <Windows.h>
#include "tim.h"

_TIM_BEGIN

class ClassEx : public WNDCLASSEX
{
public:
	ClassEx(){
		::RtlSecureZeroMemory(&(*this), sizeof(WNDCLASSEX));
	}
	ClassEx(HINSTANCE hInst, LPCTSTR clsName, UINT iconID, UINT menuID, WNDPROC proc)
	{
		::RtlSecureZeroMemory(&(*this), sizeof(WNDCLASSEX));

		hInstance = hInst;
		lpszClassName = clsName;
		hbrBackground = HBRUSH(COLOR_WINDOW+1);
		hCursor = LoadCursor(NULL, IDC_ARROW);
		cbSize = sizeof(WNDCLASSEX);
		hIcon = iconID==NULL?NULL:LoadIcon(hInst, MAKEINTRESOURCE(iconID));
		style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
		lpszMenuName = menuID==NULL?NULL:MAKEINTRESOURCE(menuID);
		lpfnWndProc = proc;
	}
	~ClassEx(){}

	void setClassName(LPCTSTR lpClsName){
		lpszClassName = lpClsName;
	}
	void setIcon(HICON hicon){
		hIcon = hicon;
	}
	void setIcon(UINT uId){
		hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(uId));
	}

	void setMenu(UINT uId){
		lpszMenuName = MAKEINTRESOURCE(uId);
	}
	void setMenu(LPCTSTR lpMenuName){
		lpszMenuName = lpMenuName;
	}
	void setStyle(UINT uStyle){
		style = uStyle;
	}
	void setBkBrush(HBRUSH hbr){
		hbrBackground = hbr;
	}
	bool Register()	{	return RegisterClassEx(&*this) ? true : false;	}
};

_TIM_END

#endif