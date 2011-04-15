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

#ifndef _MY_WINDOW_H_
#define _MY_WINDOW_H_

#include <Windows.h>
#include "tim.h"
#include "TString.h"
#include "Rect.h"

_TIM_BEGIN

class Window
{
public:
	Window():_hWnd(0), _parentWnd(0), _hinst(0){
	}

	virtual void init(HINSTANCE hinst, HWND hwndParent){
		_hinst = hinst;	_parentWnd = hwndParent;
	}

	WNDPROC Subclass(WNDPROC newProc){	
		::SetWindowLongPtr(_hWnd, GWLP_USERDATA, (LONG_PTR)this);
		return (WNDPROC)::SetWindowLongPtr(_hWnd, GWLP_WNDPROC, (LONG_PTR)newProc);	
	}

public:
	inline HWND getSelf()	const	{	return _hWnd;	}
	inline HWND GetParent()const	{	return _parentWnd;	}
	inline HWND getParent()const	{	return _parentWnd;	}
	inline void setParent(HWND hwndParent)	{	_parentWnd = hwndParent;	}
	inline HINSTANCE getHinst()const	{	return _hinst;	}

	HINSTANCE HwndToHinst(HWND hWnd)	{	return (HINSTANCE)
		::GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	}
	inline HWND SetFocus()	const	{	return ::SetFocus(_hWnd);	}

	inline int GetWindowText(LPTSTR lpText, int nMaxCount)	{	return ::GetWindowText(_hWnd, lpText, nMaxCount);	}
	inline BOOL SetWindowText(LPCTSTR lpText)	{	return ::SetWindowText(_hWnd, lpText); }

	inline int GetWindowTextLength()	{	return ::GetWindowTextLength(_hWnd);	}

	void setTitle(const TString &title)	{	SetWindowText(title.c_str());	}
	void getTitle(TString &title)
	{
		int len = GetWindowTextLength();
		TCHAR *text = new TCHAR[len+1];

		::GetWindowText(_hWnd, text, len+1);

		title = text;
	}

	TString getTitle()	{	TString t;	getTitle(t);	return t;	}

	virtual TString getText()	{	return getTitle();	}
	virtual void setText(const TString &text)	{	setTitle(text);	}

	BOOL SetWindowPos(HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags)
	{	return ::SetWindowPos(_hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);	}

	BOOL MoveWindow(int x, int y, int nWidth, int nHeight, BOOL bRepaint = TRUE){
		return ::MoveWindow(_hWnd, x, y, nWidth, nHeight, bRepaint);
	}

	void moveTo(int x, int y)	{	SetWindowPos(0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);	}
	void resizeTo(int nWidth, int nHeight)	{	SetWindowPos(0, 0, 0, nWidth, nHeight, SWP_NOMOVE | SWP_NOZORDER);	}

	void setTopMost(bool fSet = true)	{	SetWindowPos(HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);	}

	inline void hideWindow()	{	::ShowWindow(_hWnd, SW_HIDE);	}
	inline void showWindow()	{	::ShowWindow(_hWnd, SW_SHOW);	}

	inline BOOL ShowWindow(int nCmdShow)	{	return ::ShowWindow(_hWnd, nCmdShow);	}
	inline BOOL UpdateWindow()	{	return ::UpdateWindow(_hWnd);	}

	inline BOOL IsWindow()			{	return ::IsWindow(_hWnd);			}
	inline BOOL IsWindowVisible()	{	return ::IsWindowVisible(_hWnd);	}

	inline BOOL isMaximized()	{	return ::IsZoomed(_hWnd);	}
	inline BOOL isMinimized()	{	return ::IsIconic(_hWnd);	}

	inline void disable()		{	::EnableWindow(_hWnd, FALSE);	}
	inline void enable()		{	::EnableWindow(_hWnd, TRUE);	}

	void setAlphaValue(BYTE value)	{	::SetLayeredWindowAttributes(_hWnd, 0, (100-value)*255/100, LWA_ALPHA);	}

	int msgBox(TString text, TString caption = TEXT("Message Box"), UINT uType = MB_OK)
	{	return ::MessageBox(_hWnd ? _hWnd : ::GetActiveWindow(), text.c_str(), caption.c_str(), uType);	}

	LRESULT sendMsg(UINT uMsg, WPARAM wParam = 0U, LPARAM lParam = 0L)	{	return ::SendMessage(_hWnd, uMsg, wParam, lParam);	}
	inline BOOL postMsg(UINT uMg, WPARAM wParam = 0U, LPARAM lParam = 0L){
		return ::PostMessage(_hWnd, uMg, wParam, lParam);
	}
	virtual HWND create(LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
		int x = CW_USEDEFAULT, int y = 0, int nWidth = CW_USEDEFAULT, int nHeight = 0,
		int uMenuID = 0, LPVOID lpParam = 0)
	{
		return _hWnd = ::CreateWindow(lpClassName, lpWindowName, dwStyle, x, y, nWidth,\
			nHeight, _parentWnd, (HMENU)uMenuID, _hinst, lpParam);
	}

	virtual HWND createEx(DWORD dwExStyle, LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle,
		int x = CW_USEDEFAULT, int y = 0, int nWidth = CW_USEDEFAULT, int nHeight = 0,
		int uMenuID = 0, LPVOID lpParam = 0)
	{
		return _hWnd = ::CreateWindowEx(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y, nWidth,\
			nHeight, _parentWnd, (HMENU)uMenuID, _hinst, lpParam);
	}

	inline virtual void destroy()		{	::DestroyWindow(_hWnd);		}

	void centerWnd(HWND hWndParent = NULL)
	{
		if (hWndParent == NULL){
			hWndParent = GetDesktopWindow();
		}
		Rect rcParent;
		::GetClientRect(hWndParent, &rcParent);

		int x = rcParent.left + (rcParent.Width() - getWidth())/2;
		int y = rcParent.top + (rcParent.Height() - getHeight())/2;

		moveTo(x, y);
	}

	inline DWORD getStyle()	{	return (DWORD)::GetWindowLongPtr(_hWnd, GWL_STYLE);		}
	inline DWORD getExStyle()	{	return (DWORD)::GetWindowLongPtr(_hWnd, GWL_EXSTYLE);	}

	inline DWORD setStyle(DWORD dwStyle)	{
		return (DWORD)::SetWindowLongPtr(_hWnd, GWL_STYLE, dwStyle);
	}
	inline DWORD setExStyle(DWORD dwExStyle)	{
		return (DWORD)::SetWindowLongPtr(_hWnd, GWL_EXSTYLE, dwExStyle);
	}

	inline BOOL InvalidateRect(const RECT *lpRect = 0, BOOL bErase = FALSE){
		return ::InvalidateRect(_hWnd, lpRect, bErase);
	}
	inline BOOL GetClientRect(RECT *lpRect)	{	return ::GetClientRect(_hWnd, lpRect);	}
	inline BOOL GetWindowRect(RECT *lpRect)	{	return ::GetWindowRect(_hWnd, lpRect);	}

	inline BOOL ClientToScreen(LPPOINT lpPoint)	{	return ::ClientToScreen(_hWnd, lpPoint);	}
	inline BOOL ScreenToClient(LPPOINT lpPoint)	{	return ::ScreenToClient(_hWnd, lpPoint);	}

	BOOL ClientToScreen(LPRECT lpRect)
	{
		POINT lt;
		POINT rb;
		lt.x = lpRect->left;
		lt.y = lpRect->top;
		rb.x = lpRect->right;
		rb.y = lpRect->bottom;

		ClientToScreen(&lt);
		ClientToScreen(&rb);

		return SetRect(lpRect, lt.x, lt.y, rb.x, rb.y);
	}
	BOOL ScreenToClient(LPRECT lpRect)
	{
		POINT lt;
		POINT rb;
		lt.x = lpRect->left;
		lt.y = lpRect->top;
		rb.x = lpRect->right;
		rb.y = lpRect->bottom;

		ScreenToClient(&lt);
		ScreenToClient(&rb);

		return SetRect(lpRect, lt.x, lt.y, rb.x, rb.y);
	}

	enum getType {	window, client	};

	int getWidth(getType type = window)
	{
		Rect rc;
		if (type == window){
			GetWindowRect(&rc);
		}else{
			GetClientRect(&rc);
		}
		return rc.Width();
	}
	int getHeight(getType type = window)
	{
		Rect rc;
		if (type == window){
			GetWindowRect(&rc);
		}else{
			GetClientRect(&rc);
		}
		return rc.Height();
	}
	HFONT CreateFont(int cHeight = 9, int cWidth = 0, int cEscapement = 0,
		int cOrientation = 0, int cWeight = FW_DONTCARE, DWORD bItalic = FALSE,
		DWORD bUnderline = FALSE, DWORD bStrikeOut = FALSE, DWORD iCharSet = DEFAULT_CHARSET,
		DWORD iOutPrecision = OUT_DEFAULT_PRECIS, DWORD iClipPrecision = CLIP_DEFAULT_PRECIS,
		DWORD iQuality = DEFAULT_QUALITY, DWORD iPitchAndFamily = FF_DONTCARE, LPCTSTR pszFaceName = TEXT("ËÎÌו"))
	{
		return ::CreateFont(cHeight, cWeight, cEscapement, cOrientation,
			cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision,
			iClipPrecision, iQuality, iPitchAndFamily, pszFaceName);
	}

	HFONT getFont()	{	return (HFONT)sendMsg(WM_GETFONT);	}
	void  setFont(HFONT hFont)	{	sendMsg(WM_SETFONT, (WPARAM)hFont, MAKELPARAM(TRUE, 0));	}

	HMENU GetMenu()	{	return ::GetMenu(_hWnd);	}
	HMENU LoadMenu(LPCTSTR lpMenuName)	{	return ::LoadMenu(_hinst, lpMenuName);	}
	HMENU LoadMenu(UINT nIDResource)	{	return ::LoadMenu(_hinst, MAKEINTRESOURCE(nIDResource));	}

	UINT_PTR SetTimer(UINT_PTR uTimerId, UINT uElapse, TIMERPROC lpTimerFunc = NULL){
		return ::SetTimer(_hWnd, uTimerId, uElapse, lpTimerFunc);
	}
	BOOL KillTimer(UINT_PTR uTimerId){
		return ::KillTimer(_hWnd, uTimerId);
	}

	LONG SetWindowLong(int nIndex, LONG dwNewLong) {
		return ::SetWindowLong(_hWnd, nIndex, dwNewLong);
	}
	LONG GetWindowLong(int nIndex){
		return ::GetWindowLong(_hWnd, nIndex);
	}

#ifdef _WIN64
	LONG_PTR SetWindowLongPtr(int nIndex, LONG_PTR dwNewLong) {
		return ::SetWindowLongPtr(_hWnd, nIndex, dwNewLong);
	}
	LONG_PTR GetWindowLongPtr(int nIndex){
		return ::GetWindowLongPtr(_hWnd, nIndex);
	}
#endif

protected:
	HWND _hWnd;
	HWND _parentWnd;

	HINSTANCE _hinst;
};

_TIM_END

#endif