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
		return ;
	}

	if (_pConfig->exitWithPrompt())
		CheckButton(IDC_CHECK1);

	if (_pConfig->downloadWithPompt())
		CheckButton(IDC_CHECK2);

	if (_pConfig->showTaskMgr())
		CheckButton(IDC_CHECK3);

	_cbLeft.assign(IDC_BUTTON1, getSelf());
	_cbRight.assign(IDC_BUTTON2, getSelf());

	_pConfig->getColor(_crOldLeft, _crOldRight);

	_cbLeft.setColor(_crOldLeft);
	_cbRight.setColor(_crOldRight);
}

void DlgConfig::OnCommand(int id)
{
	switch (id)
	{
	case IDOK:	//确定
		_pConfig->setExitPrompt(IsButtonChecked(IDC_CHECK1));
		_pConfig->setDownPrompt(IsButtonChecked(IDC_CHECK2));
		_pConfig->setShowTaskMgr(IsButtonChecked(IDC_CHECK3));
		destroy();
		break;

	case IDCANCEL:	//取消
		sendMsg(WM_CLOSE);
		break;

	case IDC_BUTTON1:	//左颜色
	case IDC_BUTTON2:	//右颜色
		{
			COLORREF cr1, cr2;
			_pConfig->getColor(cr1, cr2);
			chooseColor(id == IDC_BUTTON1 ? cr1 : cr2);
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
