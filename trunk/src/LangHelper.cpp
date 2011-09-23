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

#include "LangHelper.h"
#include <commctrl.h>

using namespace Tim;

#define ASSERT_POINTER(p) if (p == NULL) return 

LangHelper::LangHelper()
{
	_pXmlDoc = NULL;
}

LangHelper::~LangHelper()
{
	if (_pXmlDoc){
		delete _pXmlDoc;
	}
}

bool LangHelper::Load(const TString &langFile)
{
	if (_pXmlDoc){
		delete _pXmlDoc;
	}

#ifdef UNICODE
	_pXmlDoc = new TiXmlDocument(wtoa(langFile));
#else
	_pXmlDoc = new TiXmlDocument(langFile);
#endif

	if (!_pXmlDoc)
		return false;

	return _pXmlDoc->LoadFile();
}

TString LangHelper::GetLangName()
{
	if (_pXmlDoc == NULL)
		return TEXT("");

	TiXmlElement *lang = _pXmlDoc->FirstChildElement("Language");

	if (lang)
	{
		const char *name = lang->Attribute("name");
		if (name){
			return name;
		}
	}

	return TEXT("");
}

void LangHelper::SetMainMenu(HMENU hMenu)
{
	if (!hMenu){
		return ;
	}

// 	<Menu>
// 		<Main>
// 			<Entries>

	TiXmlNode *mainMenu = _pXmlDoc->FirstChild("Language");
	ASSERT_POINTER(mainMenu);

	mainMenu = mainMenu->FirstChild("Menu");
	ASSERT_POINTER(mainMenu);

	mainMenu = mainMenu->FirstChild("Main");
	ASSERT_POINTER(mainMenu);

	SetMenuEntries(mainMenu->FirstChild("Entries"), hMenu);
	SetMenuSubEntries(mainMenu->FirstChild("SubEntries"), hMenu);
	SetCommandMenu(mainMenu->FirstChild("Command"), hMenu);
}

void LangHelper::SetUIControls(GUIWnd *parentWnd)
{
	ASSERT_POINTER(parentWnd);
	ASSERT_POINTER(_pXmlDoc);

	TiXmlNode *nodeUI = _pXmlDoc->FirstChild("Language");
	ASSERT_POINTER(nodeUI);

	nodeUI = nodeUI->FirstChild("UI");
	ASSERT_POINTER(nodeUI);

	TiXmlElement *element = nodeUI->ToElement();

	if (element)
	{
		const char *title = element->Attribute("title");
		if (title){
			parentWnd->setTitle(title);
		}
	}

	element = nodeUI->FirstChildElement("Product");
	if (element)
	{
		const char *text = element->Attribute("text");
		SetControlText(parentWnd->_labelProducts, text);
	}

	element = nodeUI->FirstChildElement("Release");
	if (element)
	{
		const char *text = element->Attribute("text");
		SetControlText(parentWnd->_labelReleases, text);
	}

	element = nodeUI->FirstChildElement("Variant");
	if (element)
	{
		const char *text = element->Attribute("text");
		SetControlText(parentWnd->_labelVariants, text);
	}

	element = nodeUI->FirstChildElement("FileList");
	if (element)
	{
		const char *text = element->Attribute("label");
		SetControlText(parentWnd->_labelFiles, text);

		TiXmlElement *item = element->FirstChildElement("FileName");
		if (item)
		{
			text = item->Attribute("text");
			if (text){
				parentWnd->_lvFile.SetColumnText(0, text);
			}
		}
		item = element->FirstChildElement("FileSize");
		if (item)
		{
			text = item->Attribute("text");
			if (text){
				parentWnd->_lvFile.SetColumnText(1, text);
			}
		}
	}

	element = nodeUI->FirstChildElement("Download");
	if (element)
	{
		const char *text = element->Attribute("text");
		SetControlText(parentWnd->_btnDownload, text);
	}
}

void LangHelper::SetControlText(Tim::Window &control, const char *text)
{
	if (text){
		control.setText(text);
	}
}

void LangHelper::SetDialog(HWND hDlg, const char * type)
{
	if (!hDlg || !_pXmlDoc)
		return ;

	TiXmlNode *nodeDialog = _pXmlDoc->FirstChild("Language");
	ASSERT_POINTER(nodeDialog);

	nodeDialog = nodeDialog->FirstChild("Dialog");
	ASSERT_POINTER(nodeDialog);
	TiXmlElement *targetDialog = nodeDialog->FirstChildElement(type);
	ASSERT_POINTER(targetDialog);

	const char *title = targetDialog->Attribute("title");
	if (title)
	{
		TString str(title);
		SetWindowText(hDlg, str);
	}

	TiXmlElement *item = targetDialog->FirstChildElement("item");

	for (; item; item = item->NextSiblingElement())
	{
		int id;

		item->Attribute("id", &id);
		const char *text = item->Attribute("text");

		if (text)
		{
			TString str(text);
			SetWindowText(GetDlgItem(hDlg, id), str);
		}
	}
}

void LangHelper::SetTaskMgr(TaskMgrWnd *pMgrWnd)
{
	ASSERT_POINTER(pMgrWnd);
	ASSERT_POINTER(_pXmlDoc);

	TiXmlNode *nodeMgrWnd = _pXmlDoc->FirstChild("Language");
	ASSERT_POINTER(nodeMgrWnd);

	nodeMgrWnd = nodeMgrWnd->FirstChild("TaskMgr");
	ASSERT_POINTER(nodeMgrWnd);

	const char *title = nodeMgrWnd->ToElement()->Attribute("title");
	if (title){
		pMgrWnd->setTitle(title);
	}

	TiXmlNode *nodeLV = nodeMgrWnd->FirstChild("ListView");
	ASSERT_POINTER(nodeLV);

	TiXmlElement *item = nodeLV->FirstChildElement("Status");
	if (item)
	{
		const char *text = item->Attribute("text");
		if (text){
			pMgrWnd->_taskView->SetColumnText(0, text);
		}
	}

	item = nodeLV->FirstChildElement("FileName");
	if (item)
	{
		const char *text = item->Attribute("text");
		if (text){
			pMgrWnd->_taskView->SetColumnText(1, text);
		}
	}

	item = nodeLV->FirstChildElement("FileSize");
	if (item)
	{
		const char *text = item->Attribute("text");
		if (text){
			pMgrWnd->_taskView->SetColumnText(2, text);
		}
	}

	item = nodeLV->FirstChildElement("Progress");
	if (item)
	{
		const char *text = item->Attribute("text");
		if (text){
			pMgrWnd->_taskView->SetColumnText(3, text);
		}
	}

	item = nodeLV->FirstChildElement("Speed");
	if (item)
	{
		const char *text = item->Attribute("text");
		if (text){
			pMgrWnd->_taskView->SetColumnText(4, text);
		}
	}
}

void LangHelper::SetNewTaskLv(HWND hDlg)
{
	if (!hDlg || !_pXmlDoc)
		return ;

	HWND hwndLV = GetDlgItem(hDlg, IDC_LIST_TASK);

	if (hwndLV == 0)
		return ;

	TiXmlNode *node = _pXmlDoc->FirstChild("Language");
	ASSERT_POINTER(node);

	node = node->FirstChild("Dialog");
	ASSERT_POINTER(node);

	node = node->FirstChild("NewTask");
	ASSERT_POINTER(node);

	node = node->FirstChild("FileList");
	ASSERT_POINTER(node);

	TiXmlElement *item = node->FirstChildElement("FileName");

	if (item)
	{
		const char *v = item->Attribute("text");
		if (v)
		{
			LVCOLUMN lvc = {0};

			TString str(v);
			lvc.mask = LVCF_TEXT;

			lvc.pszText = (TCHAR*)str.c_str();

			ListView_SetColumn(hwndLV, 0, &lvc);
		}
	}

	item = node->FirstChildElement("FileSize");

	if (item)
	{
		const char *v = item->Attribute("text");
		if (v)
		{
			LVCOLUMN lvc = {0};

			TString str(v);
			lvc.mask = LVCF_TEXT;

			lvc.pszText = (TCHAR*)str.c_str();

			ListView_SetColumn(hwndLV, 1, &lvc);
		}
	}
}

void LangHelper::SetPopupMenu(HMENU hMenu, const char *type)
{
	if (!hMenu || !type)
		return ;

	TiXmlNode *node = _pXmlDoc->FirstChild("Language");
	ASSERT_POINTER(node);

	node = node->FirstChild("Menu");
	ASSERT_POINTER(node);

	node = node->FirstChild("Popup");
	ASSERT_POINTER(node);

	node = node->FirstChild(type);
	ASSERT_POINTER(node);

	if (node->FirstChild("SubEntries"))
	{
		TiXmlElement *item = node->FirstChild("SubEntries")->FirstChildElement("item");

		for (; item; item = item->NextSiblingElement())
		{
			int /*x, */y;
			// 		item->Attribute("xPos", &x);
			item->Attribute("yPos", &y);
			const char *text = item->Attribute("text");

			if (text)
			{
#ifdef UNICODE
				ModifyMenu(hMenu, y, MF_BYPOSITION, 0, atow(text).c_str());
#else
				ModifyMenu(hMenu, y, MF_BYPOSITION, 0, text);
#endif
			}
		}
	}

	SetCommandMenu(node->FirstChild("Command"), hMenu);
}

void LangHelper::SetCommandMenu(TiXmlNode *nodeCmd, HMENU hMenu)
{
	if (!nodeCmd || !hMenu)
		return ;

	TiXmlElement *item = nodeCmd->FirstChildElement("item");

	for (; item; item = item->NextSiblingElement())
	{
		int id;
		item->Attribute("id", &id);
		const char *text = item->Attribute("text");
		if (!text)
			continue;

		int flag = MF_BYCOMMAND;
		if (::CheckMenuItem(hMenu, id, MF_BYCOMMAND) == MF_CHECKED)
			flag |= MF_CHECKED;

#ifdef UNICODE
		ModifyMenu(hMenu, id, flag, id, atow(text).c_str());
#else
		ModifyMenu(hMenu, id, flag, id, text);
#endif
	}
}

void LangHelper::SetMenuEntries(TiXmlNode *node, HMENU hMenu)
{
	if (!node || !hMenu)
		return ;

	TiXmlElement *item = node->FirstChildElement("item");

	for (; item; item = item->NextSiblingElement())
	{
		int id;
		item->Attribute("id", &id);
		const char *text = item->Attribute("text");

		if (text)
		{
#ifdef UNICODE
			ModifyMenu(hMenu, id, MF_BYPOSITION, 0, atow(text).c_str());
#else
			ModifyMenu(hMenu, id, MF_BYPOSITION, 0, text);
#endif
		}
	}
}

void LangHelper::SetMenuSubEntries(TiXmlNode *node, HMENU hMenu)
{
	if (!node || !hMenu)
		return ;

	TiXmlElement *item = node->FirstChildElement("item");

	for (; item; item = item->NextSiblingElement())
	{
		int x, y;
		item->Attribute("xPos", &x);
		item->Attribute("yPos", &y);
		const char *text = item->Attribute("text");

		HMENU hSubMenu = GetSubMenu(hMenu, x);

		if (text && hSubMenu)
		{
#ifdef UNICODE
			ModifyMenu(hSubMenu, y, MF_BYPOSITION, 0, atow(text).c_str());
#else
			ModifyMenu(hSubMenu, y, MF_BYPOSITION, 0, text);
#endif
		}
	}
}

bool LangHelper::GetConfirmInfo(const char *type, TString &title, TString &info)
{
	if (!type || !_pXmlDoc)
		return false;

	TiXmlNode *nodeDialog = _pXmlDoc->FirstChild("Language");
	if (!nodeDialog)
		return false;

	nodeDialog = nodeDialog->FirstChild("Dialog");
	if (!nodeDialog)
		return false;

	nodeDialog = nodeDialog->FirstChildElement("Confirm");
	if (!nodeDialog)
		return false;

	TiXmlElement *item = nodeDialog->FirstChildElement(type);
	if (item)
	{
		const char *v = item->Attribute("title");
		if (v)	{
			title = v;
		}
	}

	item = item->FirstChildElement("Info");
	if (item)
	{
		const char *v = item->Attribute("text");
		if (v)	{
			info = v;
		}
	}

	return true;
}

void LangHelper::GetMsgBox(const char *type, TString &text, TString &title)
{
	if (type == 0 || _pXmlDoc == 0)
		return ;

	TiXmlNode *nodeMsg = _pXmlDoc->FirstChild("Language");
	if (!nodeMsg)
		return ;

	nodeMsg = nodeMsg->FirstChild("MsgBox");
	if (!nodeMsg)
		return ;

	TiXmlElement *item = nodeMsg->FirstChildElement(type);
	if (item)
	{
		const char *v = item->Attribute("title");
		if (v){
			title = v;
		}
	}

	item = item->FirstChildElement("Text");
	if (item)
	{
		TiXmlNode *node = item->FirstChild();
		if (node)
		{
			const char *v = node->Value();
			if (v){
				text = v;
			}
		}
	}
}

int LangHelper::MsgBox(const char *type, HWND hWnd, const TString &text, const TString &title, UINT uType)
{
	if (type == 0 || _pXmlDoc == 0){
		return MessageBox(hWnd, text, title, uType);
	}

	TString strCaption = text;
	TString strTitle = title;

	GetMsgBox(type, strCaption, strTitle);

	return MessageBox(hWnd, strCaption, strTitle, uType);
}

bool LangHelper::GetStatus(LPCSTR type, TString &text)
{
	if (type == 0 || _pXmlDoc == 0)
		return false;

	TiXmlNode *node = _pXmlDoc->FirstChild("Language");
	if (!node)
		return false;

	node = node->FirstChild("Status");

	if (!node)
		return false;

	node = node->FirstChild(type);
	if (!node)
		return false;

	node = node->FirstChild();
	if (node)
	{
		const char *v = node->Value();
		if (v)
		{
			text = v;
			return true;
		}
	}

	return false;
}

bool LangHelper::GetProxyType(std::vector<TString> &type)
{
	if (_pXmlDoc == 0)
		return false;

	TiXmlNode *node = _pXmlDoc->FirstChild("Language");
	if (NULL == node)
		return false;

	node = node->FirstChild("ProxyType");

	if (NULL == node)
		return false;

	char *item_name[] = 
	{
		"NoProxy",
		"Http",
		"Sv4",
		"Sv5"
	};

	for (int i=0; i<4; i++)
	{
		TiXmlNode *item = node->FirstChild(item_name[i]);

		if (NULL == item) // all the proxy type string should have
			return false;

		item = item->FirstChild();
		if (NULL == item)
			return false;

		const char *str = item->Value();
		if (NULL == str)
			return false;

		type.push_back(str);
	}

	return true;
}