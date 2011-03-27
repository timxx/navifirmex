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

#ifndef __PROGRESS_VIEW_H__
#define __PROGRESS_VIEW_H__

#include "tim\commonctrls.h"
#include "ProgressBarEx.h"
#include <vector>
#include "data_type.h"

using namespace Tim;

class ProgressView : public ListView
{
public:
	ProgressView()
		:_lpfnOldProc(0), _iProgressColumn(1), _iSizeColumn(0)
	{}

	~ProgressView(){
		clean();
	}

	virtual HWND create(LPCTSTR lpTitle = TEXT("ListView"), HWND hwndParent = NULL,
		int id = -1, int nWidth = 0, int nHeight = 0,
		int x =0 , int y = 0, DWORD dwStyle = WS_TABSTOP | WS_VSCROLL | LVS_REPORT)
	{
		_hWnd = ListView::create(lpTitle, hwndParent, id, nWidth, nHeight, x, y, dwStyle);
		if (_hWnd){
			_lpfnOldProc = Subclass(SubProc);
		}
		return _hWnd;
	}

	void SetProgressColumn(int i){
		_iProgressColumn = i;
	}
	void SetSizeColumn(int i){
		_iSizeColumn = i;
	}

	void SetProgressPos(int i, int pos);

	int NewItem(const FileInfo &item);
	bool DelItem(int i);

	void Resize();

protected:
	static LRESULT CALLBACK SubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK ListViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void clean();
	//size: range 0 ~ size
	void NewProgress(long size);
protected:
	WNDPROC	_lpfnOldProc;
	std::vector<ProgressBarEx*> _vProgress;
	int _iProgressColumn;	//指定进度条在哪一列
	int _iSizeColumn;		//指定文件大小在哪一列
};

#endif