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

#include "DlgConfig.h"
#include "res\resource.h"
#include "nm_message.h"

BOOL DlgConfig::runProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		OnInit();
		return TRUE;

	case WM_COMMAND:
		OnCommand(LOWORD(wParam));
		break;

	case WM_CLOSE:
 		_pConfig->setColor(_crOldLeft, _crOldRight);
		::PostMessage(getParent(), NM_CHANGEUI, 0, 0);

		destroy();
		break;
	}

	return FALSE;
}

void DlgConfig::OnInit()
{
	_pConfig = (Config *)_pData;

	if (_pConfig == NULL)
	{
		msgBox(TEXT("DlgConfig::出错了"), TEXT("错误"), MB_ICONERROR);
		postMsg(WM_CLOSE);
		return ;
	}

	SendMessage(getParent(), NM_SETDIALOGLANG, (WPARAM)_hWnd, (LPARAM)"Config");

	if (_pConfig->exitWithPrompt())
		CheckButton(IDC_CHECK_EXIT);

	if (_pConfig->downloadWithPompt())
		CheckButton(IDC_CHECK_DOWNLOAD);

	if (_pConfig->showTaskMgr())
		CheckButton(IDC_CHECK_NEW_TASK);

	_cbLeft.assign(IDB_LEFT, getSelf());
	_cbRight.assign(IDB_RIGHT, getSelf());

	_pConfig->getColor(_crOldLeft, _crOldRight);

	_cbLeft.setColor(_crOldLeft);
	_cbRight.setColor(_crOldRight);
}

void DlgConfig::OnCommand(int id)
{
	switch (id)
	{
	case IDB_OK:	//确定
		_pConfig->setExitPrompt(IsButtonChecked(IDC_CHECK_EXIT));
		_pConfig->setDownPrompt(IsButtonChecked(IDC_CHECK_DOWNLOAD));
		_pConfig->setShowTaskMgr(IsButtonChecked(IDC_CHECK_NEW_TASK));
		destroy();
		break;

	case IDB_CANCEL:	//取消
		sendMsg(WM_CLOSE);
		break;

	case IDB_LEFT:	//左颜色
	case IDB_RIGHT:	//右颜色
		{
			COLORREF cr1, cr2;
			_pConfig->getColor(cr1, cr2);
			chooseColor(id == IDB_LEFT ? cr1 : cr2);
			_pConfig->setColor(cr1, cr2);

			_cbLeft.setColor(cr1);
			_cbRight.setColor(cr2);

			::PostMessage(getParent(), NM_CHANGEUI, 0, 0);
		}
		break;
	}
}

void DlgConfig::chooseColor(COLORREF &cr)
{
	COLORREF rgbCustom[16];

	for(int i = 0; i < 16; i++)
		rgbCustom[i] = RGB(255, 255, 255);

	CHOOSECOLOR cc = {0};

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.rgbResult = cr;
	cc.Flags = CC_RGBINIT | CC_FULLOPEN | CC_ANYCOLOR |CC_SOLIDCOLOR;
	cc.hwndOwner = _hWnd;
	cc.lpCustColors = rgbCustom;

	if(ChooseColor(&cc)){
		cr = cc.rgbResult;
	}
}
