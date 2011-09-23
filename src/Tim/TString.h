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

#ifndef _TSTRING_H_
#define _TSTRING_H_

#include <tchar.h>
#include <string>
#include <algorithm>

#include "tim.h"

int atow(const char *astr, wchar_t *wstr, int nDstLen = 0);
int wtoa(const wchar_t *wstr, char *astr, int nDstLen = 0);

std::wstring atow(const std::string& astr);
std::string wtoa(const std::wstring& wstr);

_TIM_BEGIN

class TString : public std::basic_string<TCHAR>
{
public:
	TString() : basic_string(){}
	TString(const TString &str) : basic_string(str){}

	TString(const wchar_t *str) : basic_string()
	{
#ifndef UNICODE
		assign(wtoa(str));
#else
		assign(str);
#endif
	}

	TString(const char *str) : basic_string()
	{
#ifdef UNICODE
		assign(atow(str));
#else
		assign(str);
#endif
	}

	TString(const std::wstring &str) : basic_string()
	{
#ifndef UNICODE
		assign(wtoa(str));
#else
		assign(str);
#endif
	}

	TString(const std::string &str) : basic_string()
	{
#ifdef UNICODE
		assign(atow(str));
#else
		assign(str);
#endif
	}

	void format(const TCHAR *fmt, ...);

	void toUpper(){
		transform (begin(), end(),  begin(), toupper);
	}

	void toLower()	{
		transform (begin(), end(),  begin(), tolower);
	}

	size_type find(const TString &findWhat, size_type offset = 0, bool caseSensitive = true);
	int compare(const TString &other, bool caseSensitive = true);

	//貌似这样用没问题。。。
	operator const TCHAR* () const{
		return c_str();
	}

// 	operator TString &(){
// 		return *this;
// 	}

	void formatV(const TCHAR* fmt, va_list args);

	// trim left:  " example" -> "example"
	// trim rigth: "exampl   " -> "example"

	TString& trimLeft();
	TString& trimRight();
	TString& trim();

	void Replace(const TString &what, const TString &with);
	//void Replace(const TCHAR &what, const TCHAR &with);

	int toInt(){
		return _ttoi(c_str());
	}
};

_TIM_END

#endif