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


#include "ProgressView.h"
#include "common.h"
#include "Tim\SException.h"

using namespace std;

LRESULT CALLBACK ProgressView::SubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ProgressView *pCtl = (ProgressView*)::GetWindowLongPtr(hWnd, GWL_USERDATA);

	return pCtl->ListViewProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ProgressView::ListViewProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
	//case WM_SIZE:
		Resize();
		break;
	}

	return CallWindowProc(_lpfnOldProc, hWnd, uMsg, wParam, lParam);
}

void ProgressView::clean()
{
	for(size_t i=0; i<_vProgress.size(); i++)
	{
		_vProgress[i]->destroy();
		delete _vProgress[i];
	}

	vector<ProgressBarEx*>().swap(_vProgress);
}

void ProgressView::SetProgressPos(int i, int pos)
{
	try
	{
		if (i >=0 && i < (int)_vProgress.size())
		{
			if (_vProgress[i]){
				_vProgress[i]->SetPos(pos);
			}
		}
	}
	catch(const Tim::SException &e)
	{
		char info[256];

		wsprintfA(info, "ProgressView::SetProgressPos: 发生[%s]异常", e.what());
		MessageBoxA(_hWnd, info, "发生异常", MB_ICONERROR);
	}
}

int ProgressView::NewItem(const FileInfo &item)
{
	int i = GetItemCount();
	InsertItem(i, TEXT(""));
	SetItemText(i, 1, (TCHAR*)item.name.c_str());
	TString size = MakeFileSizeFmt(item.size);
	SetItemText(i, 2, (TCHAR*)size.c_str());

	NewProgress(item.size);

	return i;
}

void ProgressView::NewProgress(long size)
{
	ProgressBarEx* pControl = new ProgressBarEx;

	pControl->init(getHinst(), getSelf());
	pControl->SetFont(12, TEXT("宋体"), RGB(33, 12, 226));
	pControl->create();
	pControl->SetRange32(0, size);

	_vProgress.push_back(pControl);
}

bool ProgressView::DelItem(int i)
{
	if (i < 0 || i >= (int)_vProgress.size())
		return false;
	int j = 0;

	try
	{
		for (vector<ProgressBarEx*>::iterator it = _vProgress.begin();
			it != _vProgress.end(); it++, j++)
		{
			if (j == i)
			{
				(*it)->destroy();
				delete *it;
				_vProgress.erase(it);

				break;
			}
		}
	}
	catch(const Tim::SException &e)
	{
		char info[256];

		wsprintfA(info, "ProgressView::DelItem: 发生[%s]异常", e.what());
		MessageBoxA(_hWnd, info, "发生异常", MB_ICONERROR);

	}

	return DeleteItem(i) == TRUE;
}

void ProgressView::Resize()
{
	int Top	= GetTopIndex();
	int Total = GetItemCount();
	int PerPage = GetCountPerPage();
	int LastItem=(( Top + PerPage) > Total) ? Total : Top+PerPage;

	HWND hwndHeader = GetHeader();

	for(int i=Top; i<LastItem; i++)
	{
		Rect ColRt;
		Header_GetItemRect(hwndHeader, _iProgressColumn, &ColRt);

		Rect rt;
		GetItemRect(i, &rt, LVIR_BOUNDS);

		rt.top += 1;
		rt.bottom-=1;
		rt.left += ColRt.left + 1;
		rt.right = rt.left + ColRt.Width() - 1;

		_vProgress[i]->MoveWindow(rt.left, rt.top, rt.Width(), rt.Height());
	}
}

int ProgressView::GetProgress()
{
	if (_vProgress.empty()){
		return 100;
	}

	float progress = 0;
	int size = _vProgress.size();

	try
	{
		for (size_t i=0; i<_vProgress.size(); i++)
		{
			int total =  _vProgress[i]->GetRange(FALSE, NULL);
			if (total == 0)
			{
				size--;
				continue;
			}

			progress += (float)_vProgress[i]->GetPos() / total;
		}

		if (size <= 0)
			return 100;

		//四舍五入
		int percent = int((progress * 100 / size) * 10 + 5)/10;

		return percent;
	}
	catch(const Tim::SException &e)
	{
		char info[256];

		wsprintfA(info, "ProgressView::GetProgress(): 发生[%s]异常", e.what());
		MessageBoxA(_hWnd, info, "发生异常", MB_ICONERROR);
	}

	return 100;
}
/*
void ProgressView::SetProgressRange(int i, int high)
{
	if (i < 0 || i >= (int)_vProgress.size())
		return ;

	_vProgress[i]->SetRange32(0, high);
}
*/
void ProgressView::SetProgressState(int i, int state)
{
	if (i < 0 || i >= (int)_vProgress.size())
		return ;

	_vProgress[i]->SetState(state);
}
