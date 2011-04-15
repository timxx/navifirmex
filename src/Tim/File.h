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

#ifndef __FILE_H__
#define __FILE_H__

#include <Windows.h>
#include <string>

#include "tim.h"

_TIM_BEGIN

class File
{
public:
	File();
	~File();

	enum OpenMode{ open, write, append };

	bool Open(const std::wstring &file, OpenMode mode = open);
	bool Open(const std::string &file, OpenMode mode = open);
	long Read(LPVOID lpBuffer, DWORD dwBuf);
	long Write(LPVOID lpBuffer, DWORD dwBuf);
	void Close();

	static DWORD GetFileSize(const std::wstring &file, LPDWORD pdwHigh);
	static DWORD GetFileSize(const std::string &file, LPDWORD pdwHigh);

	static BOOL Delete(const std::wstring &file);
	static BOOL Delete(const std::string &file);

	static BOOL Exists(const std::wstring &file);
	static BOOL Exists(const std::string &file);

	static std::wstring GetFileName(const std::wstring &file);

	static bool MakeDir(const std::wstring &folder);
	static bool MakeDir(const std::string &folder);

	static bool RmDir(const std::wstring &folder);
	static bool RmDir(const std::string &folder);

protected:
	HANDLE _hFile;
	std::wstring _filePath;
	
};

_TIM_END

#endif	//#ifndef __FILE_H__