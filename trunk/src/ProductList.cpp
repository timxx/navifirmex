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

#include "ProductList.h"
#include "res\resource.h"

ProductList::ProductList()
{
	_lpfnOldProc = NULL;
}

ProductList::~ProductList()
{
	if (_lpfnOldProc)
		Subclass(_lpfnOldProc);
}

LRESULT CALLBACK ProductList::ListProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ProductList *pList = (ProductList *)::GetWindowLong(hWnd, GWL_USERDATA);

	switch(uMsg)
	{
	case WM_RBUTTONDOWN:
		pList->ShowRMenu();
		break;
	}

	return CallWindowProc(pList->_lpfnOldProc, hWnd, uMsg, wParam, lParam);
}

void ProductList::ShowRMenu()
{
	HMENU hMenu = LoadMenu(IDR_MENU_PRODUCT);
	HMENU hMenuPop = GetSubMenu(hMenu, 0);

// 	::AppendMenu(hMenuPop, MF_STRING, IDM_REFRESH, TEXT("刷新产品"));
// 	::AppendMenu(hMenuPop, MF_SEPARATOR, NULL, NULL);
// 	::AppendMenu(hMenuPop, MF_STRING, IDM_EXPORT_PRODUCT, TEXT("保存列表"));

	if (GetCount() == 0)
	{
		RemoveMenu(hMenuPop, 1, MF_BYPOSITION);	//删除分隔线
		RemoveMenu(hMenuPop, 1, MF_BYPOSITION);	//删除导出列表
	}
	else if(GetCount() == 1)
		if(GetText(0) == TEXT("<空>"))
			return ;

	POINT pt;
	::GetCursorPos(&pt);

	::TrackPopupMenuEx(hMenuPop, TPM_BOTTOMALIGN, pt.x, pt.y, getParent(), 0);

	::DestroyMenu(hMenuPop);
}