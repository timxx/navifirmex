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

#ifndef HTTP_OPERATION_H
#define HTTP_OPERATION_H

#include <wininet.h>
#include <vector>
#include "TString.h"

_TIM_BEGIN

class Http
{
public:
	Http();
	~Http();

public:
	BOOL Open(LPCTSTR pszAgentName);

	BOOL PostUrl(LPCTSTR url, LPCTSTR pszPostData);
	BOOL OpenUrl(LPCTSTR url);
	char *GetPage(LPCTSTR url, BOOL fPost, LPCTSTR pszPostData, LPDWORD pdwBufferLength = NULL);
	BOOL QueryInfo(DWORD dwInfoLevel, LPVOID lpvBuffer, LPDWORD lpdwBufferLength, LPDWORD lpdwIndex = NULL){
		return HttpQueryInfo(_hRequest, dwInfoLevel, lpvBuffer, lpdwBufferLength, lpdwIndex);
	}

	void Close();

	TCHAR *MakeUrl(LPCTSTR url);
	void SetHeader(const LPCTSTR hdr){
		_hdrStr = hdr;
	}

	void FreeBuffer();

protected:
	HINTERNET	_hSession;
	HINTERNET 	_hRequest;
	HINTERNET 	_hConnect;

	TString		_hdrStr;
	std::vector<void *> _vBufferAddr;
};


_TIM_END

#endif