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

#include <shlobj.h>

#include "DlgNewTask.h"
#include "res\resource.h"
#include "nm_message.h"
#include "common.h"
#include "Tim\File.h"

using namespace Tim;

BOOL DlgNewTask::runProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		_taskMgrWnd = (TaskMgrWnd*)_pData;
		OnInit();
		return TRUE;

	case WM_COMMAND:
		OnCommand(LOWORD(wParam));
		break;

	case WM_CLOSE:
		hideWindow();
		_taskList.DeleteAllItems();
		std::vector<FileInfo>().swap(_vFileList);
		break;
	}

	return FALSE;
}

void DlgNewTask::OnInit()
{
	_taskList.assign(IDC_LIST_TASK, getSelf());

	_ctlSelAll.init(getHinst(), getSelf());
	_ctlSelReverse.init(getHinst(), getSelf());

	_ctlSelAll.create(HwndFromId(IDS_SEL_ALL), IDS_SEL_ALL);
	_ctlSelReverse.create(HwndFromId(IDS_SEL_REVERSE), IDS_SEL_REVERSE);

	_taskList.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

	_taskList.InsertColumn(0, TEXT("文件名"));
	_taskList.InsertColumn(1, TEXT("大小"));

	_taskList.SetColumnWidth(0, 270);
	_taskList.SetColumnWidth(1, 70);

	char szFolder[MAX_PATH] = {0};

	::SendMessage(getParent(), NM_GETLASTDIR, 0, (LPARAM)szFolder);

	::SendDlgItemMessageA(_hWnd, IDE_FOLDER, WM_SETTEXT, 0, (LPARAM)(szFolder));
}

bool DlgNewTask::SelectFolder(Tim::TString &folder)
{
	BROWSEINFO bi = {0};

	TCHAR	szFilePath[MAX_PATH] = {0};
	ITEMIDLIST *pidl = {0};

	bi.hwndOwner	= _hWnd;       
	bi.lpszTitle	= TEXT("请选择要保存到的文件夹");       
	bi.ulFlags		= BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE;
	bi.lpfn			= BrowseCallbackProc;

	char szFolder[MAX_PATH] = {0};

	::SendMessage(getParent(), NM_GETLASTDIR, 0, (LPARAM)szFolder);

#ifdef	UNICODE
	wchar_t wszFolder[MAX_PATH];
	atow(szFolder, wszFolder, MAX_PATH);
	bi.lParam = (LPARAM)wszFolder;
#else
	bi.lParam = (LPARAM)szFolder;
#endif

	pidl = ::SHBrowseForFolder(&bi);
	if (pidl == NULL)
		return false;

	if (!::SHGetPathFromIDList(pidl, szFilePath))
		return false;

	folder = szFilePath;
	if (!folder.empty())
	{
		if (folder.at(folder.length()-1) != TEXT('\\')){
			folder.push_back(TEXT('\\'));
		}
	}

#ifdef UNICODE
	wtoa(folder.c_str(), szFolder, MAX_PATH);
	::SendMessage(getParent(), NM_SETLASTDIR, 0, reinterpret_cast<LPARAM>(szFolder));
#else
	::SendMessage(getParent(), NM_SETLASTDIR, 0, reinterpret_cast<LPARAM>(folder.c_str()));
#endif
	return true;
}

int CALLBACK DlgNewTask::BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if   (uMsg == BFFM_INITIALIZED ){	//设置初始化目录
		SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);
	}
	return 0;
}

void DlgNewTask::AddTask(const FileInfo &file)
{
	_vFileList.push_back(file);

	int item = _taskList.GetItemCount();

	_taskList.InsertItem(item, (TCHAR*)file.name.c_str());

	TString size = MakeFileSizeFmt(file.size);

	_taskList.SetItemText(item, 1, (TCHAR*)size.c_str());

	_taskList.SetCheck(item);
}

void DlgNewTask::AddTask(const std::vector<FileInfo> &file)
{
	for (size_t i=0; i<file.size(); i++){
		AddTask(file[i]);
	}
}

void DlgNewTask::OnCommand(UINT id)
{
	switch (id)
	{
	case IDB_BROWSE:
		{
			TString folder;
			if (SelectFolder(folder) && !folder.empty()){
				SetItemText(IDE_FOLDER, folder);
			}
		}
		break;

	case IDB_DOWN_NOW:
		DownNow();
		break;

	case IDB_DOWN_CANCEL:
		sendMsg(WM_CLOSE);
		break;

	case IDS_SEL_ALL:
	case IDS_SEL_REVERSE:
		doSelection(id == IDS_SEL_ALL);
		break;
	}
}

void DlgNewTask::doSelection(bool fSelAll /* = true */)
{
	int count = _taskList.GetItemCount();
	if (count <= 0)
		return ;

	if (fSelAll)
	{
		for(int i=0; i<count; i++){
			_taskList.SetCheck(i);
		}
	}
	else
	{
		for(int i=0; i<count; i++)
		{
			if (_taskList.GetCheck(i)){
				_taskList.SetCheck(i, FALSE);
			}else{
				_taskList.SetCheck(i);
			}
		}
	}
}

void DlgNewTask::DownNow()
{
	TString folder = GetItemText(IDE_FOLDER);
	if (folder.empty())
	{
		msgBox(TEXT("您必须选择一个存放的目录！"), TEXT("提醒"), MB_ICONINFORMATION);
		return ;
	}

	Tim::File::MakeDir(folder);	//确保存在文件夹

	int count = _taskList.GetItemCount();

	if (count <= 0)
	{
		postMsg(WM_CLOSE);
		return ;
	}

	_taskMgrWnd->setFolder(folder);

	bool hasTask = false;

	for(int i=0; i<count; i++)
	{
		if (_taskList.GetCheck(i))
		{
			_taskMgrWnd->newTask(_vFileList[i]);
			hasTask = true;
		}
	}

	if (hasTask)
	{
		if (!_taskMgrWnd->IsWindowVisible())
			_taskMgrWnd->showWindow();
	}

	postMsg(WM_CLOSE);
}