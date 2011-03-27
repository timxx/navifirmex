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

//===================================================================
#include <Windows.h>
#include <tchar.h>

#include "Http.h"

#pragma comment(lib, "Wininet.lib")

//===================================================================
#define READ_SIZE 8192	//一次最大读取数据
//===================================================================
using namespace _TIM;
//===================================================================

Http::Http()
{
	_hSession = _hConnect = _hRequest = NULL;
	SetHeader(TEXT("Content-Type: application/x-www-form-urlencoded"));
}

Http::~Http()
{
	Close();
	FreeBuffer();
}

BOOL Http::Open(LPCTSTR pszAgentName)
{
	_hSession = InternetOpen(pszAgentName, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	return _hSession != NULL;
}

BOOL Http::PostUrl(LPCTSTR url, LPCTSTR pszPostData)
{
	TCHAR szAccept[] = TEXT("*/*");
	LPTSTR AcceptTypes[2]={0}; 
	AcceptTypes[0] = szAccept;

	URL_COMPONENTS uc = {0};

	uc.dwStructSize = sizeof(uc);
	uc.dwHostNameLength = 1;
	uc.dwUrlPathLength = 1;

	if (!InternetCrackUrl(url, lstrlen(url), 0, &uc)){
		return FALSE;
	}

	if (_hConnect != NULL){
		InternetCloseHandle(_hConnect);
	}

	TCHAR *HostName = NULL,
		*fileName = NULL;

	HostName = _tcsdup(uc.lpszHostName);
	HostName[uc.dwHostNameLength] = '\0';

	fileName = _tcsdup(uc.lpszUrlPath);
	fileName[uc.dwUrlPathLength] = '\0';

	if (_hRequest != NULL){
		InternetCloseHandle(_hRequest);
	}

	DWORD flags = 0;

	if (uc.nPort == 80){
		flags = INTERNET_FLAG_NO_CACHE_WRITE; 
	}else{
		flags = INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_SECURE | 
			INTERNET_FLAG_IGNORE_CERT_CN_INVALID | INTERNET_FLAG_IGNORE_CERT_DATE_INVALID; 
	}

	_hConnect = InternetConnect(_hSession, HostName, uc.nPort, NULL, NULL,
		INTERNET_SERVICE_HTTP, INTERNET_FLAG_NO_CACHE_WRITE, 0);
	if (!_hConnect){
		return FALSE;
	}
	_hRequest = HttpOpenRequest(_hConnect, TEXT("POST"), fileName,
		NULL, NULL, (LPCTSTR*)AcceptTypes, flags, 0);
	if (!_hRequest){
		return FALSE;
	}
	if(!HttpSendRequest(_hRequest, _hdrStr.c_str(), _hdrStr.length(), 
		(TCHAR*)pszPostData, lstrlen(pszPostData))) 
	{ 
		free(HostName); 
		free(fileName);

		return FALSE; 
	} 

	free(HostName); 
	free(fileName);

	return TRUE; 
}

BOOL Http::OpenUrl(LPCTSTR url)
{
	if (_hRequest != NULL){
		InternetCloseHandle(_hRequest);
	}

	_hRequest = InternetOpenUrl(_hSession, url, NULL, 0, HTTP_QUERY_DATE, 0);

	return _hRequest != NULL;
}

char *Http::GetPage(LPCTSTR url, BOOL fPost, LPCTSTR pszPostData, LPDWORD pdwBufferLength/* = NULL*/)
{
	if (url == NULL)
		return NULL;

	if (fPost){
		if (!PostUrl(url, pszPostData))
			return NULL; 
	}else{ 
		if (!OpenUrl(url))
			return NULL;
	}

	char *pszRecvData = (char*)calloc(READ_SIZE, sizeof(char));

	char buffer[READ_SIZE] = {0};
	DWORD dwRead = 0;
	int curPos = 0;

	while (InternetReadFile(_hRequest, buffer, READ_SIZE - 1, &dwRead))
	{ 
		if (dwRead == 0){
			break;
		}

		buffer[dwRead] = '\0';
		memcpy(pszRecvData + curPos ,buffer, dwRead);
		curPos += dwRead;

		pszRecvData[curPos] = '\0';

		pszRecvData = (char*)realloc(pszRecvData, curPos + READ_SIZE);
	}

	if (pdwBufferLength != NULL)
		*pdwBufferLength = curPos;

	_vBufferAddr.push_back(pszRecvData);

	return pszRecvData;
}

void Http::Close()
{
	if (_hSession != NULL){
		InternetCloseHandle(_hSession);
		_hSession = NULL;
	}
	if (_hRequest != NULL){
		InternetCloseHandle(_hRequest);
		_hRequest = NULL;
	}
	if (_hConnect != NULL){
		InternetCloseHandle(_hConnect);
		_hConnect = NULL;
	}
}

TCHAR *Http::MakeUrl(LPCTSTR url)
{
	TCHAR *buffer = NULL;
	const TCHAR *p = NULL;

	if (!url){
		return NULL;
	}

	int len = lstrlen(url) + 1;

	buffer = (TCHAR*)calloc(len * 3, sizeof(TCHAR));

	p = url;

	while (*p)
	{
		if (*p == TEXT('+')){
			lstrcat(buffer, TEXT("%2B"));

		}else if(*p == TEXT(' ')){
			lstrcat(buffer, TEXT("%20"));

		}else if(*p == TEXT('/')){
			lstrcat(buffer, TEXT("%2F"));

		}else if(*p == TEXT('?')){
			lstrcat(buffer, TEXT("%3F"));

		}else if(*p == TEXT('%')){
			lstrcat(buffer, TEXT("%25"));

		}else if(*p == TEXT('#')){
			lstrcat(buffer, TEXT("%23"));

		}else if(*p == TEXT('&')){
			lstrcat(buffer, TEXT("%26"));

		}else if(*p == TEXT('=')){
			lstrcat(buffer, TEXT("%3D"));

		}else{
			TCHAR tmp[2] = {*p};
			lstrcat(buffer, tmp);
		}
		p++;
	}

	_vBufferAddr.push_back(buffer);

	return buffer;

}

void Http::FreeBuffer()
{
	for (size_t i=0; i<_vBufferAddr.size(); i++){
		free(_vBufferAddr[i]);
	}

	_vBufferAddr.clear();
}