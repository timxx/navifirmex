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

#ifndef __MINI_DOWNLOADER_H__
#define __MINI_DOWNLOADER_H__

#include "http.h"
#include "data_type.h"

class MiniDownloader : public Http
{
public:
	MiniDownloader():Http(), _hWnd(NULL),
		_fp(NULL), _fileSize(0), _offset(0)
	{}
	~MiniDownloader();

public:
	virtual bool Init(bool fHttps, const std::string& agent = "MiniDownloader/1.0"){
		return Http::Init(false, agent);
	}

	bool Download(HWND hWnd, const TiFile &tiFile);

	bool Pause();
	bool Resume();
	void Stop();

	long GetDownloadedSize() const{
		return _fileSize;
	}

protected:
	virtual size_t ProcessData(void *data, size_t size, size_t nmemb);
	virtual int Progress(double dltotal, double dlnow, double ultotal, double ulnow);

	static DWORD WINAPI DownloadProc(LPVOID lParam);

private:
	HWND _hWnd;			//接收下载信息的窗口句柄

	FILE *_fp;	
	std::string _url;
	long _fileSize;		//记录已下载大小
	long _offset;		//记录偏移大小，在续载时恢复正常进度显示
	HANDLE _hThread;
};

#endif