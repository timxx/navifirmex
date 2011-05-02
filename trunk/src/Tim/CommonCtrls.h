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
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#ifndef _COMMON_CONTROLS_H_
#define _COMMON_CONTROLS_H_
//////////////////////////////////////////////////////////////////////////
#include "Window.h"
#include <commctrl.h>
//////////////////////////////////////////////////////////////////////////
#pragma comment(lib, "comctl32.lib")
//////////////////////////////////////////////////////////////////////////

_TIM_BEGIN
//////////////////////////////////////////////////////////////////////////
class ControlBase : public Window
{
public:
	ControlBase(){	clear();	}

	virtual void assign(UINT uId, HWND hwndParent = NULL)
	{
		_id = uId;

		if (hwndParent != NULL){
			_parentWnd = hwndParent;
		}
		if (_parentWnd != NULL){
			_hWnd = ::GetDlgItem(_parentWnd, uId);
			_hinst = HwndToHinst(_parentWnd);
		}
	}
	virtual void assign(HWND hWnd, HWND hwndParent = NULL)
	{
		_hWnd = hWnd;
		_id = ::GetDlgCtrlID(_hWnd);

		if (hwndParent != NULL){
			_parentWnd = hwndParent;
		}
		if (_parentWnd != NULL){
			_hinst = HwndToHinst(_parentWnd);
		}
	}

	void setX(int x)			{	_x = x;				}
	void setY(int y)			{	_y = y;				}
	void setWidth(int width)	{	_width = width;		}
	void setHeight(int height)	{	_height = height;	}

	void setRect(const Rect &rect){
		_x = rect.left;
		_y = rect.right;
		_width = rect.Width();
		_height = rect.Height();
	}

	void setId(int id)			{	_id = id;			}

	void setStyle(DWORD style)	{	_dwStyle = style;	}

protected:
	virtual void clear()
	{
		_x = 0;
		_y = 0;
		_width = 0;
		_height = 0;
		_id = -1;

		_dwStyle = WS_VISIBLE | WS_CHILD;
	}

protected:
	int _x;
	int _y;
	int _width;
	int _height;
	int _id;
	DWORD _dwStyle;
	TString _title;
};
//////////////////////////////////////////////////////////////////////////
class Button : public ControlBase
{
public:

	Button(){}
	//
	Button(UINT uId, HWND hwndParent = NULL){
		assign(uId, hwndParent);
	}
	Button(HWND hWnd, HWND hwndParent = NULL){
		assign(hWnd, hwndParent);
	}

public:

	virtual HWND create(LPCTSTR lpTitle = TEXT("Button"))
	{
		_hWnd = Window::create(TEXT("Button"), lpTitle, _dwStyle | WS_VISIBLE | WS_CHILD, _x, _y, _width, _height, _id);
		return _hWnd;
	}
};

class Edit : public ControlBase
{
public:
	Edit(){}
	Edit(UINT uId, HWND hwndParent = NULL){
		assign(uId, hwndParent);
	}
	Edit(HWND hWnd, HWND hwndParent = NULL){
		assign(hWnd, hwndParent);
	}

public:
	virtual HWND create(LPCTSTR lpTitle = TEXT("Edit"))
	{
		_hWnd = Window::createEx(WS_EX_CLIENTEDGE, TEXT("Edit"),
			lpTitle, _dwStyle | WS_VISIBLE | WS_CHILD, _x, _y, _width, _height, _id);

		return _hWnd;
	}

	void clear(){
		setText(TEXT(""));
	}
	bool empty() {
		return getText().empty();
	}
};

class Static : public ControlBase
{
public:
	Static(){}
	Static(UINT uId, HWND hwndParent = NULL){
		assign(uId, hwndParent);
	}
	Static(HWND hWnd, HWND hwndParent = NULL){
		assign(hWnd, hwndParent);
	}

public:
	virtual HWND create(LPCTSTR lpTitle = TEXT("Static"))
	{
		_hWnd = Window::createEx(WS_EX_TRANSPARENT, TEXT("Static"),
			lpTitle, _dwStyle | WS_VISIBLE | WS_CHILD, _x, _y, _width, _height, _id);
		return _hWnd;
	}

	void clear(){
		setText(TEXT(""));
	}
};

class ComboBox : public ControlBase
{
public:
	ComboBox(){}
	ComboBox(UINT uId, HWND hwndParent = NULL){
		assign(uId, hwndParent);
	}
	ComboBox(HWND hWnd, HWND hwndParent = NULL){
		assign(hWnd, hwndParent);
	}

	virtual HWND create(LPCTSTR lpTitle = TEXT("ComboBox"), HWND hwndParent = NULL,
		int id = -1, int nWidth = 0, int nHeight = 0,
		int x =0 , int y = 0, DWORD dwStyle = WS_TABSTOP)
	{
		_id = id;
		if(hwndParent != NULL){
			_parentWnd = hwndParent;
		}

		_hWnd = Window::create(TEXT("COMBOBOX"), lpTitle,
			dwStyle | WS_VISIBLE | WS_CHILD,
			x, y, nWidth, nHeight, id);
		return _hWnd;
	}
	int AddString(const TString &str){
		return (int)sendMsg(CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str.c_str()));
	}
	int DeleteString(int index){
		return (int)sendMsg(CB_DELETESTRING, index);
	}
	int InsertString(int index, const TString &str){
		return (int)sendMsg(CB_INSERTSTRING, index, reinterpret_cast<LPARAM>(str.c_str()));
	}
	void ResetContent(){
		sendMsg(CB_RESETCONTENT);
	}
	int SetCurSel(int nIndex){
		return (int)sendMsg(CB_SETCURSEL, nIndex, 0);
	}
	int GetCurSel(){
		return (int)sendMsg(CB_GETCURSEL);
	}
	int GetCount()	{
		return (int)sendMsg(CB_GETCOUNT);
	}
};

class CheckBox : public Button
{
public:
	CheckBox(){}
	CheckBox(UINT uId, HWND hwndParent = NULL){
		assign(uId, hwndParent);
	}
	CheckBox(HWND hWnd, HWND hwndParent = NULL){
		assign(hWnd, hwndParent);
	}

	virtual HWND create(LPCTSTR lpTitle = TEXT("CheckBox"), HWND hwndParent = NULL,
		int id = -1, int nWidth = 0, int nHeight = 0,
		int x =0 , int y = 0, DWORD dwStyle = WS_TABSTOP)
	{
		_id = id;
		if(hwndParent != NULL){
			_parentWnd = hwndParent;
		}

		_hWnd = Window::create(TEXT("BUTTON"), lpTitle,
			dwStyle | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
			x, y, nWidth, nHeight, id);
		return _hWnd;
	}

	BOOL GetCheck(){
		return sendMsg(BM_GETCHECK) == BST_CHECKED;
	}
};


class ListView : public ControlBase
{
public:
	ListView(){}
	ListView(UINT uId, HWND hwndParent = NULL){
		assign(uId, hwndParent);
	}
	ListView(HWND hWnd, HWND hwndParent = NULL){
		assign(hWnd, hwndParent);
	}

	virtual HWND create(LPCTSTR lpTitle = TEXT("ListView"), HWND hwndParent = NULL,
		int id = -1, int nWidth = 0, int nHeight = 0,
		int x =0 , int y = 0, DWORD dwStyle = WS_TABSTOP | WS_VSCROLL | LVS_REPORT)
	{
		_id = id;
		if(hwndParent != NULL){
			_parentWnd = hwndParent;
		}

		_hWnd = Window::createEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, lpTitle, dwStyle | WS_VISIBLE | WS_CHILD,
			x, y, nWidth, nHeight, id);
		return _hWnd;
	}

	inline BOOL DeleteAllItems()		{	return ListView_DeleteAllItems(_hWnd);		}
	inline BOOL DeleteItem(int iItem)	{	return ListView_DeleteItem(_hWnd, iItem);	}

	inline int GetTopIndex()			{	return ListView_GetTopIndex(_hWnd);		}
	inline int GetCountPerPage()		{	return ListView_GetCountPerPage(_hWnd);	}
	inline int  GetHotItem() const		{	return ListView_GetHotItem(_hWnd);		}
	inline BOOL GetItem(LPLVITEM pitem)			{	return ListView_GetItem(_hWnd, pitem);	}
	inline UINT GetItemState(int i, UINT mask)	{	return ListView_GetItemState(_hWnd, i, mask);}
	inline int  GetItemCount() const			{	return ListView_GetItemCount(_hWnd);	}

	inline BOOL GetSubItemRect(int iItem, int iSubItem, int code, LPRECT lpRect){
		return ListView_GetSubItemRect(_hWnd, iItem, iSubItem, code, lpRect);
	}

	void GetItemText(int iItem, int iSubItem, LPTSTR pszText, int cchTextMax){
		ListView_GetItemText(_hWnd, iItem, iSubItem, pszText, cchTextMax);
	}
	int GetSelectedColumn() const	{	return ListView_GetSelectedColumn(_hWnd);	}
	int GetSelectionMark() const	{	return ListView_GetSelectionMark(_hWnd);	}
	int GetSelectedCount() const	{	return ListView_GetSelectedCount(_hWnd);	}

	inline BOOL GetItemRect(int i, RECT *prc, int code)	{	return ListView_GetItemRect(_hWnd, i, prc, code);	}
	BOOL GetColumn(int iCol, LPLVCOLUMN pcol)			{	return ListView_GetColumn(_hWnd, iCol, pcol);		}

	int InsertItem(const LPLVITEM pitem)	{	return ListView_InsertItem(_hWnd, pitem);	}
	int InsertItem(int iItem, LPTSTR lpszItem)
	{
		LVITEM lvi = {0};
		lvi.iItem = iItem;
		lvi.mask = LVIF_TEXT;
		lvi.cchTextMax = lstrlen(lpszItem);
		lvi.pszText = lpszItem;

		return InsertItem(&lvi);
	}
	BOOL SetColumn(int iCol, LPLVCOLUMN pcol)	{	return ListView_SetColumn(_hWnd, iCol, pcol);	}
	BOOL SetColumnText(int iCol, const TString &text)
	{
		LVCOLUMN lvc = {0};
		lvc.mask = LVCF_TEXT;
		lvc.pszText = (TCHAR*)text.c_str();

		return SetColumn(iCol, &lvc);
	}

	BOOL SetItem(const LPLVITEM pitem)		{	return ListView_SetItem(_hWnd, pitem);		}

	void SetItemText(int iItem, int iSubItem, LPTSTR pszText)
	{	ListView_SetItemText(_hWnd, iItem, iSubItem, pszText);	}

	BOOL SetBkColor(COLORREF clrBk)	{	return ListView_SetBkColor(_hWnd, clrBk);	}

	BOOL SetTextColor(COLORREF clrText)		{	return ListView_SetTextColor(_hWnd, clrText);	}
	BOOL SetTextBkColor(COLORREF clrTextBk)	{	return ListView_SetTextBkColor(_hWnd, clrTextBk);	}

	int InsertColumn(int iCol, const LPLVCOLUMN pcol)	{	return ListView_InsertColumn(_hWnd, iCol, pcol);	}
	int InsertColumn(int iCol, LPTSTR lpColName)
	{
		LVCOLUMN lvc = {0};

		lvc.mask = LVCF_TEXT;
		lvc.pszText = lpColName;

		return InsertColumn(iCol, &lvc);
	}

	void SetExtendedStyle(DWORD dwExStyle)	{	ListView_SetExtendedListViewStyle(_hWnd, dwExStyle);	}

	BOOL SetColumnWidth(int iCol, int iWidth = LVSCW_AUTOSIZE_USEHEADER)
	{	return ListView_SetColumnWidth(_hWnd, iCol, iWidth);	}

	HIMAGELIST SetImageList(HIMAGELIST  himl, int iImageList) const{
		return ListView_SetImageList(_hWnd, himl, iImageList);
	}

	HWND GetHeader() const {	return ListView_GetHeader(_hWnd);	}

	void SetCheck(int nIndex, BOOL fChecked = TRUE){
		ListView_SetCheckState(_hWnd, nIndex, fChecked);
	}

	BOOL GetCheck(int nIndex){
		return ListView_GetCheckState(_hWnd, nIndex);
	}

	BOOL EnsureVisible(int index, BOOL fPartialOK = TRUE){
		return (BOOL)sendMsg(LVM_ENSUREVISIBLE, index, fPartialOK);
	}
	HWND SetToolTips(HWND hwndTip){
		return (HWND)sendMsg(LVM_SETTOOLTIPS, 0, (LPARAM)hwndTip);
	}
	HWND GetToolTips(){
		return (HWND)sendMsg(LVM_GETTOOLTIPS);
	}
	DWORD SetHoverTime(DWORD dwHoverTime){
		return (DWORD)sendMsg(LVM_SETHOVERTIME, 0, dwHoverTime);
	}
	BOOL SetInfoTip(PLVSETINFOTIP plvSetInfoTip){
		return (BOOL)sendMsg(LVM_SETINFOTIP, 0, reinterpret_cast<LPARAM>(plvSetInfoTip));
	}
};

class ListBox : public ControlBase
{
public:
	ListBox(){}
	ListBox(UINT uId, HWND hwndParent = NULL){
		assign(uId, hwndParent);
	}
	ListBox(HWND hWnd, HWND hwndParent = NULL){
		assign(hWnd, hwndParent);
	}

	virtual HWND create(LPCTSTR lpTitle = TEXT("ListBox"))
	{
		_hWnd = Window::createEx(WS_EX_CLIENTEDGE, 
			WC_LISTBOX, lpTitle, _dwStyle | WS_VISIBLE | WS_CHILD,
			_x, _y, _width, _height, _id);
		return _hWnd;
	}

	int AddString(const TString &str)	{
		return (int)sendMsg(LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(str.c_str()));
	}
	int DeleteString(int index)	{
		return (int)sendMsg(LB_DELETESTRING, index);
	}
	int GetCurSel(){
		return (int)sendMsg(LB_GETCURSEL);
	}
	int GetCount(){
		return (int)sendMsg(LB_GETCOUNT);
	}
	void ResetContent(){
		sendMsg(LB_RESETCONTENT);
	}
	int SetCurSel(int index){
		return (int)sendMsg(LB_SETCURSEL, index);
	}
	void SetHorizontalExtent(UINT nExtent){
		sendMsg(LB_SETHORIZONTALEXTENT, nExtent);
	}
	UINT GetHorizontalExtent(){
		return sendMsg(LB_GETHORIZONTALEXTENT);
	}
	int GetTextLen(int index){
		return (int)sendMsg(LB_GETTEXTLEN, index);
	}
	int GetText(int index, LPTSTR lpBuffer){
		return sendMsg(LB_GETTEXT, index, reinterpret_cast<LPARAM>(lpBuffer));
	}
	TString GetText(int index)
	{
		int len = GetTextLen(index);

		TCHAR *buffer = new TCHAR[len+1];

		GetText(index, buffer);

		TString str(buffer);

		delete [] buffer;

		return str;
	}
};

class StatusBar : public ControlBase
{
public:
	StatusBar(){}
	StatusBar(UINT uId, HWND hwndParent = NULL){
		assign(uId, hwndParent);
	}
	StatusBar(HWND hWnd, HWND hwndParent = NULL){
		assign(hWnd, hwndParent);
	}

	virtual HWND create(LPCTSTR lpTitle = TEXT(""), HWND hwndParent = NULL,
		int id = -1, DWORD dwStyle = WS_VISIBLE | WS_CHILD)
	{
		_id = id;
		if(hwndParent != NULL){
			_parentWnd = hwndParent;
		}

		_hWnd = CreateStatusWindow(dwStyle, lpTitle, _parentWnd, id);

		return _hWnd;
	}
	virtual BOOL setText(int iPart, const TString &text){
		return (BOOL)sendMsg(SB_SETTEXT, iPart, reinterpret_cast<LPARAM>(text.c_str()));
	}
	BOOL SetParts(int nParts, int aWidths[]){
		return (BOOL)sendMsg(SB_SETPARTS, nParts, reinterpret_cast<LPARAM>(aWidths));
	}
	BOOL GetRect(int nPart, LPRECT lpRect){
		return (BOOL)sendMsg(SB_GETRECT, nPart, (LPARAM)lpRect);
	}
};

class ProgressCtl : public ControlBase
{
public:
	ProgressCtl(){}
	ProgressCtl(UINT uId, HWND hwndParent = NULL){
		assign(uId, hwndParent);
	}
	ProgressCtl(HWND hWnd, HWND hwndParent = NULL){
		assign(hWnd, hwndParent);
	}

	virtual HWND create(LPCTSTR lpTitle = TEXT("Progress Bar"), HWND hwndParent = NULL,
		int id = -1, int nWidth = 0, int nHeight = 0,
		int x =0 , int y = 0, DWORD dwStyle = PBS_SMOOTH)
	{
		_id = id;
		if(hwndParent != NULL){
			_parentWnd = hwndParent;
		}

		_hWnd = Window::create(PROGRESS_CLASS, lpTitle,
			dwStyle | WS_CHILD | WS_VISIBLE,
			x, y,nWidth, nHeight, id);

		return _hWnd;
	}

	int DeltaPos(int nIncrement){
		return (int)sendMsg(PBM_DELTAPOS, nIncrement);
	}
	COLORREF GetBarColor(){
		return (COLORREF)sendMsg(PBM_GETBARCOLOR);
	}
	COLORREF GetBkColor(){
		return (COLORREF)sendMsg(PBM_GETBKCOLOR);
	}
	UINT GetPos(){
		return (UINT)sendMsg(PBM_GETPOS);
	}
	int GetRange(BOOL fWhichLimit, PPBRANGE ppbRange){
		return (int)sendMsg(PBM_GETRANGE, fWhichLimit, reinterpret_cast<LPARAM>(ppbRange));
	}
	int GetState(){
		return (int)sendMsg(PBM_GETSTATE);
	}
	int GetStep(){
		return (int)sendMsg(PBM_GETSTEP);
	}
	COLORREF SetBarColor(COLORREF cr){
		return (COLORREF)sendMsg(PBM_SETBARCOLOR, 0, (LPARAM)cr);
	}
	COLORREF SetBkColor(COLORREF cr){
		return (COLORREF)sendMsg(PBM_SETBKCOLOR, 0, (LPARAM)cr);
	}
	BOOL SetMarquee(BOOL fOn, UINT time){
		return (BOOL)sendMsg(PBM_SETMARQUEE, fOn, time);
	}
	int SetPos(int pos){
		return (int)sendMsg(PBM_SETPOS, pos);
	}
	int SetRange(int nMinRange = 0, int nMaxRange = 100){
		return (int)sendMsg(PBM_SETRANGE, 0, MAKELPARAM(nMinRange, nMaxRange));
	}
	DWORD SetRange32(int iLowLim = 0, int iHighLim = 100){
		return (DWORD)sendMsg(PBM_SETRANGE32, iLowLim, iHighLim);
	}
	int SetState(int iState){
		return (int)sendMsg(PBM_SETSTATE, iState);
	}
	int SetStep(int nStepInc){
		return (int)sendMsg(PBM_SETSTEP, nStepInc);
	}
	int StepIt(){
		return (int)sendMsg(PBM_STEPIT);
	}
};

_TIM_END

#endif