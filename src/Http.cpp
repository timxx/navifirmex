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

//disable signed/unsigned mismatch
#pragma warning(disable:4018)

#include "Http.h"

Http::Http()
{
	_curl = NULL;
	_pfnProgress = NULL;
	_pUser = NULL;
	_strHeader = "Content-Type: application/x-www-form-urlencoded";
	_fHttps = false;
}

Http::~Http()
{
	CleanBuffer();
	Close();
}

size_t Http::write_func(void *ptr, size_t size, size_t nmemb, void *pHttp)
{
	return ((Http*)pHttp)->ProcessData(ptr, size, nmemb);
}
size_t Http::ProcessData(void *data, size_t size, size_t nmemb)
{
	if (!data){
		return 0;
	}

	size_t realsize = size * nmemb;

	_buffer.memory = (char*)realloc(_buffer.memory, _buffer.size + realsize + 1);

	memcpy(&(_buffer.memory[_buffer.size]), data, realsize);

	_buffer.size += realsize;
	_buffer.memory[_buffer.size] = 0;

// 	if (pHttp->_pfnProgress != NULL){
// 		pHttp->_pfnProgress(pHttp->_lenContent, buffer->size, pHttp->_pUser);
// 	}

	return realsize;
}

// size_t Http::GetContentLength(void *ptr, size_t size, size_t nmemb, void *pLen)
// {
// 	sscanf_s((const char*)ptr, "Content-Length: %ld\n", (DWORD*)pLen);
// 
// 	return size * nmemb;  
// }

int Http::progress_callback(void *pHttp, double dltotal, double dlnow, double ultotal, double ulnow)
{
	return ((Http*)pHttp)->Progress(dltotal, dlnow, ultotal, ulnow);
}

int Http::Progress(double dltotal, double dlnow, double ultotal, double ulnow)
{
	if (_pfnProgress != NULL)
		_pfnProgress(dltotal, dlnow, _pUser);

	return 0;
}

void Http::CleanBuffer()
{
	for (size_t i=0; i<_vBuffer.size(); i++)
		if (_vBuffer[i].memory != NULL)
			free(_vBuffer[i].memory);

	std::vector<Buffer>().swap(_vBuffer);
}

bool Http::Init(bool fHttps, const std::string& agent/* = "Http/1.0"*/)
{
	_curl = curl_easy_init();

	if (_curl == NULL){
		return false;
	}

	_fHttps = fHttps;
	return curl_easy_setopt(_curl, CURLOPT_USERAGENT, agent.c_str()) == CURLE_OK;
}

void Http::Close()
{
	if (_curl != NULL)
	{
		curl_easy_cleanup(_curl);
		_curl = NULL;
	}
}

HTTPRESULT_CODE Http::Get(const std::string& url, bool fIncudeHeader/* = false*/)
{
	curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &write_func);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1 );
	if (_fHttps)
	{
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 0L);
	}

	if (fIncudeHeader){
		curl_easy_setopt(_curl, CURLOPT_HEADER, 1L);
	}

// 	curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, GetContentLength);  
// 	curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &_dwSize);  

	curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
	curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, this);

	CURLcode ret = curl_easy_perform(_curl);

	if (_buffer.memory && _buffer.size != 0)
	{
		_vBuffer.push_back(_buffer);

		_buffer.size = 0;
		_buffer.memory = NULL;
	}

	return ret;
}

HTTPRESULT_CODE Http::Post(const std::string& url, LPVOID lpData)
{
	curl_easy_setopt(_curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &write_func);
	curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
	curl_easy_setopt(_curl, CURLOPT_POST, 1 );
	if (_fHttps)
	{
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 0L);
	}

	curl_easy_setopt(_curl, CURLOPT_NOPROGRESS, 0L);
	curl_easy_setopt(_curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
	curl_easy_setopt(_curl, CURLOPT_PROGRESSDATA, this);

// 	curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, GetContentLength);  
// 	curl_easy_setopt(_curl, CURLOPT_HEADERDATA, &_dwSize);  

	curl_slist *chunk = NULL;
	chunk = curl_slist_append(chunk, _strHeader.c_str());

	curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, chunk);
	curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, lpData);

	CURLcode ret = curl_easy_perform(_curl);

	if (_buffer.memory && _buffer.size != 0)
	{
		_vBuffer.push_back(_buffer);

		_buffer.size = 0;
		_buffer.memory = NULL;
	}

	curl_slist_free_all(chunk);

	return ret;
}

char* Http::GetData(LPDWORD pdwSize, int index/* = -1*/)
{
	//impotant to cast _vBuffer.size() to int!!!!
	if (_vBuffer.empty() || index >= (int)_vBuffer.size())
		return NULL;

	if (index == -1){
		index = _vBuffer.size() - 1;
	}

	for (size_t i=0; i<_vBuffer.size(); i++)
	{
		if (i == index)
		{
			if (pdwSize != 0){
				*pdwSize = _vBuffer[i].size;
			}

			return  _vBuffer[i].memory;
		}
	}

	return NULL;
}