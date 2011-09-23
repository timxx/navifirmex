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

#include <Windows.h>
#include <vector>
#include "TinyXml\tinyxml.h"
#include "Tim\TString.h"
#include "GUIWnd.h"
#include "TaskMgrWnd.h"

class LangHelper
{
public:
	LangHelper();
	~LangHelper();

	bool Load(const TString &langFile);

	Tim::TString GetLangName();

	void SetMainMenu(HMENU hMenu);
	void SetPopupMenu(HMENU hMenu, const char *type);
	void SetUIControls(GUIWnd *parentWnd);
	void SetTaskMgr(TaskMgrWnd *pMgrWnd);
	void SetDialog(HWND hDlg, const char *type);
	void SetNewTaskLv(HWND hDlg);

	bool GetConfirmInfo(const char *type, TString &title, TString &info);

	void GetMsgBox(const char *type, TString &text, TString &title);
	int MsgBox(const char *type, HWND hWnd, const TString &text, const TString &title, UINT uType);

	bool GetStatus(LPCSTR type, TString &text);

	bool GetProxyType(std::vector<TString> &type);

protected:
	void SetControlText(Tim::Window &control, const char *text);
	void SetCommandMenu(TiXmlNode *nodeCmd, HMENU hMenu);
	void SetMenuEntries(TiXmlNode *node, HMENU hMenu);
	void SetMenuSubEntries(TiXmlNode *node, HMENU hMenu);
private:

	TiXmlDocument *_pXmlDoc;
};