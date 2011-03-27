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
#include "ImageEx.h"
#include <process.h>
////////////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "Gdiplus.lib")
////////////////////////////////////////////////////////////////////////////////
using namespace Gdiplus;
////////////////////////////////////////////////////////////////////////////////
ImageEx::ImageEx(HINSTANCE hInstance, LPCTSTR lpResName, LPCTSTR lpResType)
{
	Init();

	Load(hInstance, lpResName, lpResType);
}
////////////////////////////////////////////////////////////////////////////////
ImageEx::ImageEx(LPCWSTR filename, BOOL useEmbeddedColorManagement)
	: Image(filename, useEmbeddedColorManagement/* = FALSE*/)
{
	Init();

	_fIsInitialized = true;

	TestForAnimatedGIF();
}

////////////////////////////////////////////////////////////////////////////////
ImageEx::~ImageEx()
{
	Destroy();
}
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::InitAnimation(HWND hWnd, int x, int y)
{
	_hWnd = hWnd;
	_pt.x = x;
	_pt.y = y;

	if (!_fIsInitialized)
		return false;

	if (IsGIF())
	{
		if (_hThread == NULL)
		{	
			unsigned int nTID = 0;

			_hThread = (HANDLE) _beginthreadex( NULL, 0, AnimationProc, this, CREATE_SUSPENDED, &nTID);
			
			if (!_hThread){
				return true;
			}else{
				ResumeThread(_hThread);
			}
		}
	}
	return false;
}
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::LoadFromBuffer(BYTE* pBuff, int nSize)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	if (hGlobal)
	{
		void* pData = GlobalLock(hGlobal);
		if (pData)
			memcpy(pData, pBuff, nSize);
		
		GlobalUnlock(hGlobal);

		if (CreateStreamOnHGlobal(hGlobal, TRUE, &_pStream) == S_OK)
			bResult = true;
	}

	return bResult;
}

////////////////////////////////////////////////////////////////////////////////
int ImageEx::GetResource(HINSTANCE hInstance, LPCTSTR lpResName, LPCTSTR lpResType, LPVOID lpResBuffer)
{ 
	int			nLen	= 0;
	bool		bResult	= FALSE;

	HRSRC hResInfo = FindResource(hInstance , lpResName, lpResType);
	if (!hResInfo)
		return 0;

	HGLOBAL hResData = LoadResource(hInstance , hResInfo);

	if (!hResData) 
		return 0;

	LPVOID lpRes = LockResource(hResData);
	if (!lpRes)
		return 0;

	DWORD dwSize = SizeofResource(hInstance, hResInfo);

	if (!lpResBuffer)
		return dwSize;

	memcpy(lpResBuffer, lpRes, dwSize);

	return dwSize;
}
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::Load(HINSTANCE hInstance, LPCTSTR lpResName, LPCTSTR lpResType)
{
	BYTE *pBuff = NULL;

	int	 nSize = GetResource(hInstance, lpResName, lpResType, NULL);

	if (nSize <= 0)
		return false;

	pBuff = new BYTE[nSize];

	GetResource(hInstance, lpResName, lpResType, pBuff);

	bool fResult = LoadFromBuffer(pBuff, nSize);

	delete [] pBuff;

	_fIsInitialized = fResult;

	if (fResult)
	{
		lastResult = DllExports::GdipLoadImageFromStreamICM(_pStream, &nativeImage);

		TestForAnimatedGIF();
	}

	return fResult;
}
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::TestForAnimatedGIF()
{
   UINT count = 0;
   count = GetFrameDimensionsCount();
   GUID* pDimensionIDs = new GUID[count];

   // Get the list of frame dimensions from the Image object.
   GetFrameDimensionsList(pDimensionIDs, count);

   // Get the number of frames in the first dimension.
   _nFrameCount = GetFrameCount(&pDimensionIDs[0]);

	// Assume that the image has a property item of type PropertyItemEquipMake.
	// Get the size of that property item.
   int nSize = GetPropertyItemSize(PropertyTagFrameDelay);

   // Allocate a buffer to receive the property item.
   _pPropertyItem = (PropertyItem*) malloc(nSize);

   GetPropertyItem(PropertyTagFrameDelay, nSize, _pPropertyItem);
 
   delete [] pDimensionIDs;

   return _nFrameCount > 1;
}

////////////////////////////////////////////////////////////////////////////////
void ImageEx::Init()
{
	nativeImage = NULL;

	_pStream = NULL;
	_nFramePosition = 0;
	_nFrameCount = 0;
	_pStream = NULL;
	lastResult = InvalidParameter;
	_hThread = NULL;
	_fIsInitialized = false;
	_pPropertyItem = NULL;
	
	_fPause = false;
	_hWnd = NULL;
	_pt.x = _pt.y = 0;

	_hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	_hPause = CreateEvent(NULL, TRUE, TRUE, NULL);
}

////////////////////////////////////////////////////////////////////////////////
UINT WINAPI ImageEx::AnimationProc(LPVOID pParam)
{
	ImageEx *pImage = reinterpret_cast<ImageEx *> (pParam);

	pImage->AnimationThread();

	return 0;
}
////////////////////////////////////////////////////////////////////////////////
void ImageEx::AnimationThread()
{
	_nFramePosition = 0;

	while (DrawFrameGIF() == false)
		;
}
////////////////////////////////////////////////////////////////////////////////
bool ImageEx::DrawFrameGIF()
{
	::WaitForSingleObject(_hPause, INFINITE);

	GUID   pageGuid = FrameDimensionTime;
	
	HDC hdc = GetDC(_hWnd);

	Graphics graphics(hdc);

	graphics.DrawImage(this, _pt.x, _pt.y, GetWidth(), GetHeight());

	ReleaseDC(_hWnd, hdc);

	SelectActiveFrame(&pageGuid, _nFramePosition++);		
	
	if (_nFramePosition == _nFrameCount)
		_nFramePosition = 0;

	long lPause = ((long*) _pPropertyItem->value)[_nFramePosition] * 10;

	DWORD dwErr = WaitForSingleObject(_hExitEvent, lPause);

	return dwErr == WAIT_OBJECT_0;
}
////////////////////////////////////////////////////////////////////////////////
void ImageEx::Pause()
{
	if (!IsGIF())
		return;

	::ResetEvent(_hPause);

	_fPause = true;
}
////////////////////////////////////////////////////////////////////////////////
void ImageEx::Play()
{
	if (!IsGIF())
		return;

	::SetEvent(_hPause);

	_fPause = false;
}
////////////////////////////////////////////////////////////////////////////////
void ImageEx::Destroy()
{
	if (_hThread)
	{
		Pause();

		SetEvent(_hExitEvent);
		WaitForSingleObject(_hThread, 100/*INFINITE*/);
	}

	CloseHandle(_hThread);
	CloseHandle(_hExitEvent);
	CloseHandle(_hPause);

	free(_pPropertyItem);

 	_pPropertyItem = NULL;
 	_hThread = NULL;
 	_hExitEvent = NULL;
 	_hPause = NULL;

	if (_pStream)
		_pStream->Release();
}
////////////////////////////////////////////////////////////////////////////////