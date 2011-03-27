//////////////////////////////////////////////////////////////////////////
#include "TextScroller.h"
//////////////////////////////////////////////////////////////////////////
using namespace _TIM;
//////////////////////////////////////////////////////////////////////////
TextScroller::TextScroller()
{
	_iCurY	= 0;
	_fFirstTime	= true;
	_hFont = NULL;

	_crBackgnd = RGB(255, 255, 255);
	_fDecreasePos = true;
}
//////////////////////////////////////////////////////////////////////////
TextScroller::~TextScroller()
{
	if (_hFont){
		DeleteObject(_hFont);
	}
}
//////////////////////////////////////////////////////////////////////////
void TextScroller::OnPaint()
{
	KillTimer(1);

	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(_hWnd, &ps);

	DrawBackgnd(hdc);

	EndPaint(_hWnd, &ps);
}

void TextScroller::DrawBackgnd(HDC hdc, const Rect& rcInvalid /* = Rect */)
{
	Rect rcClient;
	GetClientRect(&rcClient);

	if (_fFirstTime)
	{
		if (!_lines.empty())
		{
			std::list<Line>::iterator it =  _lines.begin();
			Line& line = *it;

			HGDIOBJ hOldObj = SelectObject(hdc, _hFont);
		}
	}
}

void TextScroller::OnTimer(UINT nTimerID)
{
	MoveTextToTop();
}

void TextScroller::MoveTextToTop()
{

}

void TextScroller::SetScrollTime(int nElapse)
{
	if (nElapse > 0)
	{
		KillTimer(1);
		SetTimer(1, nElapse);
	}
}

void TextScroller::SetText(const TString& str)
{
	if (str.empty())
		return ;

	TString strLines = str;

	int pos = strLines.find(TEXT("\r\n"));
	if (pos == -1)	//只有一行
	{
		Line line;
		line.strLine = str;
		_lines.push_back(line);
	}
	else
	{
		size_t i = 0;

		while (i < strLines.length())
		{
			Line line;

			while (i < strLines.length() && strLines.at(i) != TEXT('\r') && strLines.at(i) != TEXT('\n'))
				line.strLine.push_back(strLines.at(i++));

			//skip \r\n
			if (strLines.at(i) == TEXT('\r'))
			{
				if (strLines.at(++i) == TEXT('\n'))
					i++;
			}//skip \n\r (uncommon case)
			else if (strLines.at(i) == TEXT('\n'))
			{
				if (strLines.at(++i) == TEXT('\r'))
					i++;
			}

			_lines.push_back(line);
		}
	}

	InvalidateRect(NULL, TRUE);
}