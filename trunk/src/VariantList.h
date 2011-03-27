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

#ifndef __VARIANT_LIST_H__
#define __VARIANT_LIST_H__

#include "tim\commonctrls.h"

using namespace Tim;

class VariantList : public ListBox
{
public:
	VariantList();
	~VariantList();

	virtual void assign(UINT uId, HWND hwndParent = NULL)
	{
		ListBox::assign(uId, hwndParent);

		_lpfnOldProc = Subclass(ListProc);
	}

protected:
	static LRESULT CALLBACK ListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void ShowRMenu();
protected:
	WNDPROC _lpfnOldProc;
};

#endif