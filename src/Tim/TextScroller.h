//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#ifndef __TEXT_SCROLLER_H__
#define __TEXT_SCROLLER_H__
//////////////////////////////////////////////////////////////////////////
#include <list>

#include "window.h"
#include "tim.h"
//////////////////////////////////////////////////////////////////////////
_TIM_BEGIN
//////////////////////////////////////////////////////////////////////////
struct TextInfo
{
	COLORREF	crText;
	TString		faceName;
	int			iWidth;
	int			iHeigth;
	bool		fBold;
	bool		fItalic;

	TextInfo()
	{
		crText = RGB(0, 0, 0);
		faceName = TEXT("ËÎÌו");
		iWidth = 0;
		iHeigth = 12;
	}

	TextInfo& operator = (const TextInfo& ti)
	{
		this->crText = ti.crText;
		this->faceName = ti.faceName;
		this->iWidth = ti.iWidth;
		this->iHeigth = iHeigth;
		this->fBold = ti.fBold;
		this->fItalic = ti.fItalic;

		return *this;
	}
};

struct Line
{
	TString	strLine;
	int		iPos;
	int		iInitPos;
	bool	fDecrease;

	Line()
	{
		iPos = iInitPos = 0;
		fDecrease = true;
	}
};
//////////////////////////////////////////////////////////////////////////
class TextScroller : public Window
{
public:
	TextScroller();
	~TextScroller();

public:
	void SetTextInfo(const TextInfo& ti){
		_textInfo = ti;
	}
	void SetBorder(bool fBorder = true);
	void SetTextColor(COLORREF color);
	void SetScrollTime(int nElapse);
	void SetBkColor(COLORREF color);
	void SetText(const TString& str);

protected:
	void FormatLines();
	void MoveTextToTop();
	void DrawBackgnd(HDC hdc, const Rect& rcInvalid = Rect(0, 0, 0, 0));

	void OnPaint();
	void OnTimer(UINT nTimerID);
protected:
	std::list<Line>	_lines;
	TextInfo		_textInfo;

	int		_iCurY;
	bool	_fFirstTime;
	HFONT	_hFont;

	COLORREF _crBackgnd;
	bool	 _fDecreasePos;
};
//////////////////////////////////////////////////////////////////////////
_TIM_END
//////////////////////////////////////////////////////////////////////////
#endif	//__TEXT_SCROLLER_H__
//////////////////////////////////////////////////////////////////////////