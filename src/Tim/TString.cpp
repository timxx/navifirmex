/*
http://code.google.com/p/vcardtool/
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

//disable signed/unsigned mismatch
#pragma warning(disable:4018)

#include <Windows.h>

#include "TString.h"

using namespace _TIM;

int atow(const char *astr, wchar_t *wstr, int nDstLen)
{
	if (wstr == NULL)
		return MultiByteToWideChar( CP_ACP, 0, astr, -1, NULL, 0 );

	RtlZeroMemory(wstr, nDstLen);

	return MultiByteToWideChar( CP_ACP, 0, astr, -1, wstr, nDstLen );
}

int wtoa(const wchar_t *wstr, char *astr, int nDstLen)
{
	if (astr == NULL)
		return WideCharToMultiByte( CP_ACP, 0, wstr, -1, astr, 0, NULL, NULL );

	RtlZeroMemory(astr, nDstLen);

	return WideCharToMultiByte(CP_ACP, 0, wstr, -1, astr, nDstLen, NULL, NULL);
}

std::wstring atow(const std::string& astr)
{
	wchar_t *wstr = NULL;
	int len = MultiByteToWideChar(CP_ACP, 0, astr.c_str(), astr.length(), NULL, 0) + 1;

	wstr = new wchar_t[len];
	MultiByteToWideChar( CP_ACP, 0, astr.c_str(), astr.length(), wstr, len);

	wstr[len-1] = 0;

	std::wstring str = wstr;

	delete [] wstr;

	return str;
}

std::string wtoa(const std::wstring& wstr)
{
	char *astr = NULL;

	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), NULL, 0, NULL, NULL) + 1;

	astr = new char[len];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.length(), astr, len, NULL, NULL);

	astr[len-1] = 0;

	std::string str = astr;
	delete [] astr;
	return str;
}

void TString::formatV(const TCHAR* fmt, va_list args)
{
	int nLength = _vsctprintf(fmt, args) + 1;

	if (nLength > _Mysize)
		resize(nLength);
	//如果当前小于默认的_BUF_SIZE时this->_Bx._Ptr为0
	_vstprintf_s(this->_BUF_SIZE <= this->_Myres ? this->_Bx._Ptr : this->_Bx._Buf, nLength, fmt, args );
}

void TString::format(const TCHAR* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	formatV(fmt, args);
	va_end(args);
}

TString::size_type TString::find(const TString &findWhat, size_type offset/* = 0*/, bool caseSensitive/* = false*/)
{
	TString thisStr = *this;
	TString fwStr = findWhat;

	if (!caseSensitive){
		thisStr.toUpper();
		fwStr.toUpper();
	}
	basic_string<TCHAR> tmp = thisStr;

	return tmp.find(fwStr, offset);
}

int TString::compare(const TString &other, bool caseSensitive /* = true */)
{
	TString thisStr = *this;
	TString oteStr = other;

	if (!caseSensitive){
		thisStr.toUpper();
		oteStr.toUpper();
	}

	basic_string<TCHAR> tmp = thisStr;

	return tmp.compare(oteStr);
}

TString& TString::trimLeft()
{
	if (!empty())
	{
		iterator psz = begin();
		while ((psz != end()) && (_istspace(*psz)))	//fixed isspace to _istspace. Apr. 10, 2011
			++psz;

		erase(begin(), psz);
	}

	return *this;
}

TString& TString::trimRight()
{
	if (!empty())
	{
		reverse_iterator psz = rbegin();
		while ((psz != rend()) && (_istspace(*psz)))
			++psz;

		erase(psz.base(), end());
	}
	return *this;
}

TString& TString::trim()
{
	return trimRight().trimLeft();
}

void TString::Replace(const TString &what, const TString &with)
{
	if (what.empty())
		return ;

	while(true)
	{
		int pos = find(what);
		if (pos == TString::npos)
			break;

		replace(pos, what.length(), with);
	}
}
/*
void TString::Replace(const TCHAR &what, const TCHAR &with)
{
	replace(begin( ), end( ), what , with);
}
*/