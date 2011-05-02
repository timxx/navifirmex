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

#include "common.h"

using namespace Tim;

HBITMAP GradienBitmap(HWND hWnd, COLORREF cr1, COLORREF cr2)
{
	Rect rcClient;
	::GetClientRect(hWnd, &rcClient);

	HDC hdc = GetDC(hWnd);

	int nRed	= GetRValue(cr2) - GetRValue(cr1),
		nGreen	= GetGValue(cr2) - GetGValue(cr1),
		nBlue	= GetBValue(cr2) - GetBValue(cr1);

	float fRefStep = 0.0,
		fGreenStep = 0.0,
		fBlueStep  = 0.0;

	//取最大的色差
	int nSteps = max(abs(nRed), max(abs(nGreen), abs(nBlue)));

	//确定每一颜色填充多大的矩形区域
	float fStep = (float)(rcClient.Width() + 5)/(float)nSteps;

	//设置每一颜色填充的步数
	fRefStep	= nRed/(float)nSteps;
	fGreenStep	= nGreen/(float)nSteps;
	fBlueStep	= nBlue/(float)nSteps;

	nRed	= GetRValue(cr1);
	nGreen	= GetGValue(cr1);
	nBlue	= GetBValue(cr1);

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP hbmp = CreateCompatibleBitmap(hdc, rcClient.Width(), rcClient.Height());

	HGDIOBJ hOldObj = SelectObject(hdcMem, hbmp);

	if (cr1 == cr2)
	{
		HBRUSH hbr = CreateSolidBrush(cr1);
		FillRect(hdcMem, &rcClient, hbr);
		DeleteObject(hbr);
	}
	else
	{
		Rect rcFill;

		for(int nOnBand=0; nOnBand < nSteps; nOnBand++)
		{
			SetRect(&rcFill, int(nOnBand * fStep),
				0, int((nOnBand+1)*fStep), rcClient.bottom + 1);

			HBRUSH hbr = CreateSolidBrush(
				RGB( nRed + fRefStep * nOnBand,
				nGreen + fGreenStep * nOnBand,
				nBlue + fBlueStep * nOnBand )
				);

			HGDIOBJ hOldBrush = SelectObject(hdcMem, hbr);

			FillRect(hdcMem, &rcFill, hbr);

			SelectObject(hdcMem, hOldBrush);
			DeleteObject(hbr);
		}
	}

	SelectObject(hdcMem, hOldObj);
	DeleteDC(hdcMem);
	ReleaseDC(hWnd, hdc);

	return hbmp;
}

void DrawBitmap(const HBITMAP &hBitmap, HDC hdc, const Rect &rect)
{
	if (!hBitmap || !hdc){
		return;
	}

	HDC hdcMem = CreateCompatibleDC(hdc);
	HGDIOBJ hOldObj = SelectObject(hdcMem, hBitmap);

	BitBlt(hdc, rect.left, rect.top, rect.Width(), rect.Height(), hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, hOldObj);
	DeleteDC(hdcMem);
}

bool CopyTextToClipbrd(LPCTSTR lpData)
{
	if (!OpenClipboard(NULL))
		return false;

	bool fOk = false;
	EmptyClipboard();

	int len = lstrlen(lpData);

	if (len > 0)
	{
		HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, (len + 1) * sizeof(TCHAR));

		if (hglbCopy == NULL)
			goto _exit;

		LPTSTR  lptstrCopy = (LPTSTR)GlobalLock(hglbCopy);

		memcpy(lptstrCopy, lpData, len * sizeof(TCHAR));

		GlobalUnlock(hglbCopy);
		UINT format = CF_UNICODETEXT;

#ifndef UNICODE
		format = CF_TEXT;
#endif
		if (SetClipboardData(format, hglbCopy)==NULL)
			goto _exit;

		fOk = true;
	}

_exit:
	CloseClipboard();

	return fOk;
}

//////////////////////////////////////////////////////////////////////////
TString MakeFileSizeFmt(DWORD dwSize)
{
	TCHAR szUnit[3] = TEXT("B");

	float size = (float)dwSize;

	if (size >= 1024 && size < 1024 * 1024)//KB
	{
		size /= 1024;
		lstrcpy(szUnit, TEXT("KB"));
	}
	else if (size >= 1024 *1024 && size < 1024 * 1024 *1024)//MB
	{
		size /= 1024 *1024;
		lstrcpy(szUnit, TEXT("MB"));
	}

	TString sizeFmt;
	sizeFmt.format(TEXT("%.2f %s"), size, szUnit);

	return sizeFmt;
}