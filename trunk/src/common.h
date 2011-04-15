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

#ifndef __COMMON_H__
#define __COMMON_H__

#include <Windows.h>
#include "Tim/Rect.h"
#include "Tim/TString.h"


HBITMAP GradienBitmap(HWND hWnd, COLORREF cr1, COLORREF cr2);
void DrawBitmap(const HBITMAP &hBitmap, HDC hdc, const Tim::Rect &rect);
bool CopyTextToClipbrd(LPCTSTR lpData);

Tim::TString MakeFileSizeFmt(DWORD dwSize);

#endif	//__COMMON_H__