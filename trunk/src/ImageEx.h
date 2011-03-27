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
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
#include <Windows.h>
#include <GdiPlus.h>
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

namespace Gdiplus
{
	class ImageEx : public Image
	{
	public:
		ImageEx(){	Init();	}
		ImageEx(HINSTANCE hInstance, LPCTSTR lpResName, LPCTSTR lpResType);
		ImageEx(LPCWSTR filename, BOOL useEmbeddedColorManagement = FALSE);

		~ImageEx();
	public:
		bool	Load(HINSTANCE hInstance, LPCTSTR lpResName, LPCTSTR lpResType);

		bool	IsGIF() const { return _nFrameCount > 1; }
		void	Pause();
		void	Play();
		bool	IsPaused() const { return _fPause; }
		bool	InitAnimation(HWND hWnd, int x, int y);
		void	Destroy();

	protected:

		bool	TestForAnimatedGIF();
		void	Init();
		bool	DrawFrameGIF();

		bool	LoadFromBuffer(BYTE* pBuff, int nSize);
		int		GetResource(HINSTANCE hInstance, LPCTSTR lpResName, LPCTSTR lpResType, LPVOID lpResBuffer);

		void	AnimationThread();

		static	UINT WINAPI AnimationProc(LPVOID pParam);

		HANDLE			_hThread;
		HANDLE			_hPause;
		HANDLE			_hExitEvent;

		UINT			_nFrameCount;
		UINT			_nFramePosition;

		bool			_fIsInitialized;
		bool			_fPause;

		PropertyItem *	_pPropertyItem;
		IStream *		_pStream;

		HWND	_hWnd;
		POINT	_pt;
	};

}