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

#ifndef __HTTP_H__
#define __HTTP_H__

#include <Windows.h>
#include <string>
#include <vector>

#include "curl/curl.h"

#define HTTPRESULT_CODE	CURLcode

struct Buffer
{
	char*	memory;
	DWORD	size;

	Buffer()
	{
		memory = 0;
		size = 0;
	}
};

class Http
{
public:
	Http();
	~Http();

	virtual bool Init(bool fHttps, const std::string& agent = "Http/1.0");
	void Close();

public:
	HTTPRESULT_CODE Get(const std::string& url, bool fIncudeHeader = false);
	HTTPRESULT_CODE Post(const std::string& url, LPVOID lpData);

	//取得index的数据，-1为最后一次的
	char* GetData(LPDWORD pdwSize, int index = -1);
	void SetHeader(const std::string& hdr){
		_strHeader = hdr;
	}
	void CleanBuffer();

	typedef void (*PROGRESSCALLBACK)(double, double, void*);

	void ShowProgress(PROGRESSCALLBACK pfnCallback, void* user_p)
	{
		_pfnProgress = pfnCallback;
		_pUser = user_p;
	}

protected:
	static size_t write_func(void *ptr, size_t size, size_t nmemb, void *pHttp);
	virtual size_t ProcessData(void *data, size_t size, size_t nmemb);

	static int progress_callback(void *pHttp, double dltotal, double dlnow, double ultotal, double ulnow);
	virtual int Progress(double dltotal, double dlnow, double ultotal, double ulnow);

	//static size_t GetContentLength(void *ptr, size_t size, size_t nmemb, void *pLen);

protected:
	Buffer _buffer;	//
	std::vector<Buffer> _vBuffer;
	CURL*	_curl;	//

	std::string _strHeader;
	bool	_fHttps;

	PROGRESSCALLBACK _pfnProgress;
	void*	_pUser;
	//double	_lenContent;	//Content-Length
};

#endif	//__HTTP_H__