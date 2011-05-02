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

#ifndef __DLG_NEW_TASK_H__
#define __DLG_NEW_TASK_H__

#include <vector>
#include "tim\dialog.h"
#include "Tim\CommonCtrls.h"
#include "Tim\LinkCtrl.h"
#include "data_type.h"
#include "TaskMgrWnd.h"

class DlgNewTask : public Tim::Dialog
{
public:
	DlgNewTask():_taskMgrWnd(0){}

public:
	void AddTask(const FileInfo &file);
	void AddTask(const std::vector<FileInfo> &file);

	//resize the "Select All" & "Inverse Select" link
	void ResizeLabels();

protected:
	virtual BOOL CALLBACK runProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnInit();
	void OnCommand(UINT id);

	void doSelection(bool fSelAll = true);

	bool SelectFolder(Tim::TString &folder);
	static int CALLBACK BrowseCallbackProc(HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

	void DownNow();
private:
	Tim::ListView _taskList;

	std::vector<FileInfo> _vFileList;

	Tim::LinkCtrl _ctlSelAll;
	Tim::LinkCtrl _ctlSelReverse;

	TaskMgrWnd *_taskMgrWnd;
};

#endif	//__DLG_NEW_TASK_H__