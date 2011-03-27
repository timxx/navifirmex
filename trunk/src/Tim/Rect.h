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

#ifndef _RECT_H_
#define _RECT_H_

#include "tim.h"

_TIM_BEGIN

class Rect : public RECT
{
public:
	Rect(){
		left = top = right = bottom = 0;
	}
	Rect(const Rect &rect){	CopyRect(rect);	}
	Rect(int iLeft, int iTop, int iRight, int iBottom)	{	SetRect(iLeft, iTop, iRight, iBottom);	}
	Rect(const POINT &ptLt, const POINT &ptRb){
		left = ptLt.x;
		top = ptLt.y;
		right = ptRb.x;
		bottom = ptRb.y;
	}
	~Rect(){}

	BOOL CopyRect(const RECT &rect)	{	return ::CopyRect(this, &rect);	}
	void Clear()	{	left = top = right = bottom = 0;	}
	void SetRect(int l, int t, int r, int b){
		left = l; top = t; right = r; bottom = b;
	}
	void SetRect(const RECT *pRc){
		left = pRc->left;
		top = pRc->top;
		right = pRc->right;
		bottom = pRc->bottom;
	}

	int Width()		const	{	return right - left;	}
	int Height()	const	{	return bottom - top;	}

	bool IsRectEmpty() const {	return (Width() == 0 || Height() == 0);	}
	BOOL PtInRect(const POINT &pt) const {	return ::PtInRect(this, pt);	}

	void NormalizeRect()
	{
		int nTemp;
		if (this->left > this->right)
		{
			nTemp = this->left;
			this->left = this->right;
			this->right = nTemp;
		}
		if (this->top > this->bottom)
		{
			nTemp = this->top;
			this->top = this->bottom;
			this->bottom = nTemp;
		}
	}

	void DeflateRect(int l, int t, int r, int b)
	{
		left += l;
		top += t;
		right -= r;
		bottom -= b;
	}

	bool operator == (const Rect &rect){
		return (
			left	== rect.left	&&
			top		== rect.top		&&
			right	== rect.right	&&
			bottom	== rect.bottom
			);
	}

	Rect & operator = (const Rect &rect){
		this->SetRect(&rect);
		return *this;
	}
	//以中心扩大offset
	Rect & operator += (int offset){
		this->left -= offset;
		this->top -= offset;
		this->right += offset;
		this->bottom += offset;
		return *this;
	}

	Rect & operator  += (const Rect &rect){
		this->left	 += rect.left;
		this->top	 += rect.top;
		this->right  += rect.right;
		this->bottom += rect.bottom;
		return *this;
	}

	Rect & operator -= (int offset){
		this->left += offset;
		this->top += offset;
		this->right -= offset;
		this->bottom -= offset;
		return *this;
	}

	//以中心扩大
	Rect & operator *= (int offset){
		this->left /= offset;
		this->top /= offset;
		this->right *= offset;
		this->bottom *= offset;
		return *this;
	}


};

class Point : public POINT
{
public:
	Point(){
		x = y = -1;
	}

	Point(long xx, long yy){
		x = xx;
		y = yy;
	}
	Point(const Point &pt){
		x = pt.x;
		y = pt.y;
	}

	Point(LPARAM lParam){
		x = LOWORD(lParam);
		y = HIWORD(lParam);
	}

	bool operator == (const Point &pt){
		return x == pt.x && y == pt.y;
	}

	bool InRect(const Rect &rect){
		return ::PtInRect(&rect, *this) == TRUE;
	}

	bool InRect(const RECT *pRect){
		return ::PtInRect(pRect, *this) == TRUE;
	}

	Point & operator = (const Point &pt){
		this->x = pt.x;
		this->y = pt.y;
		return *this;
	}
};

_TIM_END

#endif