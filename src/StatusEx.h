
#pragma once

#include "tim/CommonCtrls.h"

class StatusEx : public Tim::StatusBar
{
public:
	StatusEx()
	{
		_hbmpProcess = 0;
		_processIndex = 0;
		_lpfnOldProc = NULL;
	}

	~StatusEx()
	{
		if (_hbmpProcess)
			DeleteObject(_hbmpProcess);
		if (_lpfnOldProc)
			Subclass(_lpfnOldProc);
	}

	//must call before create
	virtual void init(HINSTANCE hinst, HWND hwndParent);

	virtual HWND create()
	{
		_hWnd = StatusBar::create();
		if (_hWnd){
			_lpfnOldProc = Subclass(SubProc);
		}

		return _hWnd;
	}

	void Play()		{	SetTimer(1, 20);	}
	void Pause()	{	KillTimer(1);		}
protected:
	static LRESULT CALLBACK SubProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:

	int _processIndex;
	HBITMAP _hbmpProcess;

	WNDPROC _lpfnOldProc;
};

