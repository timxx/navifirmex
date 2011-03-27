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

#include "DlgNew.h"
#include "res\resource.h"
#include "nm_message.h"

using namespace _TIM;

BOOL CALLBACK DlgNew::runProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		SetDefaultButton(IDOK);
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			TString str = GetItemText(IDE_CODE_URL);
			if (str.empty())
			{
				msgBox(TEXT("请先输入完整信息"), TEXT("提示"), MB_ICONINFORMATION);
				FocusCtrl(IDE_CODE_URL);
				break;
			}

			bool url = IsUrl(str);
			//用Post str会被释放了
			//但Send又会阻塞。。干脆由接收那边释放好了
			TCHAR *buffer = new TCHAR[str.length()+1];
			lstrcpy(buffer, str.c_str());
			::PostMessage(getParent(), url ? NM_URLDOWN : NM_CODEDOWNLOAD, 0, reinterpret_cast<LPARAM>(buffer));

			destroy();
		}
		break;

	case WM_CLOSE:
		destroy();
		break;
	}

	return FALSE;
}

bool DlgNew::IsUrl(const TString &str)
{
	//str不能为空

	TString tmp = str;
	//只要出现 : \ / http ftp . 之类都算是URL
	tmp.trim();
	tmp.toLower();
	int npos = TString::npos;

	TCHAR url[] = TEXT(":.\\/_-");
	
	for (size_t i = 0; i<tmp.length(); i++)
	{
		for (size_t j=0; j<6; j++)
			if (tmp.at(i) == url[j])
				return true;
	}

	if (tmp.find(TEXT("http")) != npos)
		return true;

	if (tmp.find(TEXT("ftp")) != npos)
		return true;

	return false;
}