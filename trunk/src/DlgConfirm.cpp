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

#include "DlgConfirm.h"
#include "res\resource.h"

BOOL CALLBACK DlgConfirm::runProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			SetDefaultButton(IDNO);
			//不知为啥加下面这行后主窗口的X也不起作用了。。。
			//SetClassLongPtr(_hWnd, GCL_STYLE, CS_NOCLOSE);

			if (!_infoText.empty())
				SetItemText(IDS_INFO, _infoText);

			if (!_title.empty()){
				SetWindowText(_title);
			}
		}
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDYES)
		{
			_fYes = true;
			sendMsg(WM_CLOSE);
		}
		else if (LOWORD(wParam) == IDNO)
		{
			_fYes = false;
			sendMsg(WM_CLOSE);
		}
		break;

		//禁止SC_CLOSE及其它。。。
	case WM_SYSCOMMAND:
		return TRUE;

	case WM_CLOSE:
		_fPrompt = !IsButtonChecked(IDC_CHECK1);
		destroy();
		break;
	}

	return FALSE;
}