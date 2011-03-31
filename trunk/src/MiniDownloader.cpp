
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

#include "MiniDownloader.h"
#include "nm_message.h"

MiniDownloader::~MiniDownloader()
{
	if (_fp){
		fclose(_fp);
	}
	if (_hThread){
		CloseHandle(_hThread);
	}
}

size_t MiniDownloader::ProcessData(void *data, size_t size, size_t nmemb)
{
	size_t written = fwrite(data, size, nmemb, _fp);

	_fileSize += written;

	return written;
}

bool MiniDownloader::Download(HWND hWnd, const TiFile &tiFile)
{
	_hWnd = hWnd;

	if (tiFile.offset > 0)
	{
		curl_easy_setopt(_curl, CURLOPT_RESUME_FROM/*_LARGE*/, tiFile.offset);
		fopen_s(&_fp, tiFile.file, "ab+");
	}
	else
	{
		fopen_s(&_fp, tiFile.file, "wb+");
	}

	if (_fp == NULL){
		return false;
	}

	_offset = _fileSize = tiFile.offset;
	_url = tiFile.url;

	DWORD dwThreadId;
	_hThread = CreateThread(NULL, 0, DownloadProc, this, 0, &dwThreadId);
	if (_hThread == INVALID_HANDLE_VALUE)
		return false;

	return true;
}

DWORD WINAPI MiniDownloader::DownloadProc(LPVOID lParam)
{
	MiniDownloader *pMD = (MiniDownloader*)lParam;

	HTTPRESULT_CODE ret = pMD->Get(pMD->_url);
	BOOL bSucceed = (ret == CURLE_OK);

	fclose(pMD->_fp);
	pMD->_fp = NULL;

	//::PostMessage(pMD->_hWnd, NM_DOWNFINISH, bSucceed, reinterpret_cast<LPARAM>(pMD));
	::SendMessage(pMD->_hWnd, NM_DOWNFINISH, bSucceed, reinterpret_cast<LPARAM>(pMD));

	return 0;
}

int MiniDownloader::Progress(double dltotal, double dlnow, double ultotal, double ulnow)
{
	DownloadStatus status((long)dltotal, (long)dlnow + _offset, this);

	//::PostMessage(_hWnd, NM_DOWNPROGRESS, 0, reinterpret_cast<LPARAM>(&status));
	::SendMessage(_hWnd, NM_DOWNPROGRESS, 0, reinterpret_cast<LPARAM>(&status));

	return 0;
}

bool MiniDownloader::Pause()
{
	if (_hThread == INVALID_HANDLE_VALUE){
		return false;
	}

	if (SuspendThread(_hThread) == (DWORD)-1){
		return false;
	}
	return true;
}

bool MiniDownloader::Resume()
{
	if (_hThread == INVALID_HANDLE_VALUE){
		return false;
	}

	if (ResumeThread(_hThread) == (DWORD)-1){
		return false;
	}

	return true;
}

void MiniDownloader::Stop()
{
	if (_hThread != INVALID_HANDLE_VALUE)
	{
		TerminateThread(_hThread, 0);
		CloseHandle(_hThread);

		_hThread = INVALID_HANDLE_VALUE;

		Close();
	}
}