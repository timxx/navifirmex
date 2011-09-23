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
#include <vector>

#include "DlgProxy.h"
#include "res/resource.h"
#include "Tim/CommonCtrls.h"
#include "nm_message.h"

BOOL DlgProxy::runProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		OnInit();
		return TRUE;

	case WM_COMMAND:
		OnCommand(LOWORD(wParam), HWND(lParam), HIWORD(wParam));
		break;

	case WM_CLOSE:
		destroy();
		break;
	}

	return FALSE;
}

void DlgProxy::OnInit()
{
	_pConfig = (Config *)_pData;
	if (NULL == _pConfig)
	{
		msgBox(TEXT("DlgProxy::出错了，无法传递配置！"), TEXT("错误"), MB_ICONERROR);
		postMsg(WM_CLOSE);
		return ;
	}

	std::vector<TString> proxy_type;

	SendMessage(getParent(), NM_SETDIALOGLANG, (WPARAM)_hWnd, (LPARAM)"Proxy");
	SendMessage(getParent(), NM_GETPROXYTYPE, 0, (LPARAM)&proxy_type);

	ComboBox cb(IDC_COMBO_PROXY_TYPE, _hWnd);

	if (proxy_type.size() != 4)
	{
		cb.AddString(TEXT("不使用代理"));
		cb.AddString(TEXT("HTTP代理"));
		cb.AddString(TEXT("SOCKS v4代理"));
		cb.AddString(TEXT("SOCKS v5代理"));
	}
	else
	{
		for(UINT i=0; i<4; i++)
			cb.AddString(proxy_type[i]);
	}

	Proxy proxy;

	_pConfig->getProxy(proxy);

	if (proxy.type < 0 || proxy.type >= 4)
		proxy.type = 0;

	cb.SetCurSel(proxy.type);
	EnableControls(proxy.type != 0);

	TString str;
	str.format(TEXT("%d"), proxy.port);

	SetItemText(IDE_PROXY_ADDR, proxy.server);
	SetItemText(IDE_PROXY_PORT, str);
	SetItemText(IDE_PROXY_USER, proxy.usr);
	SetItemText(IDE_PROXY_PWD, proxy.pwd);
}

void DlgProxy::OnCommand(int id, HWND hwndCtl, UINT uNotifyCode)
{
	switch(id)
	{
	case IDB_PROXY_CANCEL:
		sendMsg(WM_CLOSE);
		break;

	case IDB_PROXY_OK:
		{
			Proxy proxy;

			proxy.type = SendItemMsg(IDC_COMBO_PROXY_TYPE, CB_GETCURSEL);
			proxy.server = GetItemText(IDE_PROXY_ADDR);
			proxy.port = GetItemText(IDE_PROXY_PORT).toInt();
			proxy.usr = GetItemText(IDE_PROXY_USER);
			proxy.pwd = GetItemText(IDE_PROXY_PWD);

			_pConfig->setProxy(proxy);

// 			if (msgBox(TEXT("代理设置完成，是否重启程序生效？"),
// 				TEXT("重启生效？"), MB_ICONQUESTION | MB_YESNO) == IDYES)
// 			{
// 				STARTUPINFO si;
// 				PROCESS_INFORMATION pi;
// 
// 				GetStartupInfo(&si);
// 				TString cmdLine = GetCommandLine();
// 
// 				if (cmdLine.find("--restart") == TString::npos)
// 					cmdLine += TEXT(" --restart");	// add the restart flag
// 
// 				CreateProcess(NULL, (TCHAR*)cmdLine.c_str(), NULL, NULL, FALSE,
// 					NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
// 
// 				::PostQuitMessage(0);
// 			}

			destroy();
		}
		break;

	case IDC_COMBO_PROXY_TYPE:
		if (uNotifyCode == CBN_SELCHANGE)
		{
			ComboBox cb(IDC_COMBO_PROXY_TYPE, _hWnd);

			EnableControls(cb.GetCurSel() != 0);
		}
		break;
	}
}

void DlgProxy::EnableControls(bool fEnable /* = true */)
{
	::EnableWindow(HwndFromId(IDE_PROXY_ADDR), fEnable);
	::EnableWindow(HwndFromId(IDE_PROXY_PORT), fEnable);

	::EnableWindow(HwndFromId(IDE_PROXY_USER), fEnable);
	::EnableWindow(HwndFromId(IDE_PROXY_PWD), fEnable);
}