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
#include "Dialog.h"

using namespace _TIM;

void Dialog::create(UINT uID, LPVOID lParam/* = 0*/)
{
	_pData = lParam;

	_hWnd = ::CreateDialogParam(_hinst, MAKEINTRESOURCE(uID), _parentWnd, (DLGPROC)dlgProc, (LPARAM)this);

	if(!_hWnd)	return ;

	_fModeless = true;
	_isCreated = true;

	showWindow();
}

void Dialog::doModal(UINT uID, LPVOID lParam/* = 0*/)
{
	_pData = lParam;
	_isCreated = true;
	_nResult = ::DialogBoxParam(_hinst,  MAKEINTRESOURCE(uID), _parentWnd, (DLGPROC)dlgProc, (LPARAM)this);
}

BOOL CALLBACK Dialog::dlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		case WM_INITDIALOG :
		{
			Dialog *pDlg = (Dialog *)(lParam);
			pDlg->_hWnd = hDlg;
			::SetWindowLongPtr(hDlg, GWLP_USERDATA, (LONG_PTR)lParam);

            pDlg->runProc(uMsg, wParam, lParam);
			
			return TRUE;
		}

		default :
		{
			Dialog *pDlg = reinterpret_cast<Dialog *>(::GetWindowLongPtr(hDlg, GWL_USERDATA));

			if (!pDlg)	return FALSE;

			return pDlg->runProc(uMsg, wParam, lParam);
		}
	}
}

TString Dialog::GetItemText(int id)
{
	TCHAR *text = 0;

	HWND hwndItem = HwndFromId(id);

	if (!hwndItem)	return TEXT("");

	int len = ::GetWindowTextLength(hwndItem);

	text  = new TCHAR[len+1];

	::RtlSecureZeroMemory(text, len+1);

	::GetWindowText(hwndItem, text, len+1);

	return text;
}