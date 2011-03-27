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

#include <Shlwapi.h>

#include "File.h"
#include "TString.h"

#pragma comment(lib, "shlwapi.lib")

using namespace _TIM;

File::File(void)
{
	_hFile = INVALID_HANDLE_VALUE;
}

File::~File(void)
{
	Close();
}

bool File::Open(const std::wstring &file, OpenMode mode /* = open */)
{
	_hFile = CreateFileW(file.c_str(), GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL, mode == write ? CREATE_ALWAYS : OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (_hFile == INVALID_HANDLE_VALUE){
		return false;
	}

	if (mode == append){
		SetFilePointer(_hFile, 0, 0, FILE_END);
	}

	return true;
}

bool File::Open(const std::string &file, OpenMode mode /* = open */)
{
	return Open(atow(file), mode);
}

long File::Read(LPVOID lpBuffer, DWORD dwBuf)
{
	if (_hFile == INVALID_HANDLE_VALUE){
		return 0;
	}

	DWORD dwRead;
	ReadFile(_hFile, lpBuffer, dwBuf, &dwRead, NULL);

	return dwRead;
}

long File::Write(LPVOID lpBuffer, DWORD dwBuf)
{
	if (_hFile == INVALID_HANDLE_VALUE){
		return 0;
	}

	DWORD dwWritten;

	WriteFile(_hFile, lpBuffer, dwBuf, &dwWritten, NULL);

	return dwWritten;
}

void File::Close()
{
	if (_hFile)
	{
		CloseHandle(_hFile);
		_hFile = INVALID_HANDLE_VALUE;
	}
}

DWORD File::GetFileSize(const std::wstring &file, LPDWORD pdwHigh)
{
	HANDLE hFile = CreateFileW(file.c_str(), GENERIC_ALL,
		FILE_SHARE_READ, NULL,
		OPEN_EXISTING, NULL, NULL);

	if (hFile == INVALID_HANDLE_VALUE){
		return INVALID_FILE_SIZE;
	}

	DWORD low = ::GetFileSize(hFile, pdwHigh);
	CloseHandle(hFile);

	return low;
}

DWORD File::GetFileSize(const std::string &file, LPDWORD pdwHigh)
{
	return GetFileSize(atow(file), pdwHigh);
}

BOOL File::Delete(const std::wstring &file)
{
	return DeleteFileW(file.c_str());
}

BOOL File::Delete(const std::string &file)
{
	return Delete(atow(file));
}

BOOL File::Exists(const std::wstring &file)
{
	return PathFileExistsW(file.c_str());
}

BOOL File::Exists(const std::string &file)
{
	return Exists(atow(file));
}

std::wstring File::GetFileName(const std::wstring &file)
{
	if (file.empty()){
		return L"";
	}
// 	int size = GetFileTitleW(file.c_str(), NULL, 0);
// 
// 	if (size > 0)
// 	{
// 		wchar_t *buffer = new wchar_t[size + 1];
// 		GetFileTitleW(file.c_str(), buffer, size + 1);
// 
// 		std::wstring str = buffer;
// 
// 		delete [] buffer;
// 
// 		return str;
// 	}
// 
// 	return L"";

	std::wstring str = file;

	int pos = str.rfind(L'\\');

	if (pos == std::wstring::npos)
	{
		pos = str.rfind(L'/');
		if (pos == std::wstring::npos){
			return L"";
		}
	}

	//若最后一个字符为'\'
	if (pos + 1 >= (int)str.length()){
		return L"";
	}

	return str.substr(pos + 1);
}