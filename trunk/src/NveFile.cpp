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

#include "NveFile.h"
#include "Tim/File.h"

using namespace std;

bool NveFile::load(const Tim::TString &nveFile)
{
	Tim::File file;

	if (!file.Open(nveFile)){
		return false;
	}

	int size = sizeof(TiFile);
	long read;

	while(true)
	{
		TiFile tiFile;

		read = file.Read(&tiFile, size);
		if (read != size)
			break;

		_nvFileList.push_back(tiFile);
	}

	return true;
}

bool NveFile::save()
{
	if (_nvFileList.empty()){
		return false;
	}

	if (_filePath.empty()){
		return false;
	}

	Tim::File file;
	if (!file.Open(_filePath, Tim::File::write)){
		return false;
	}

	list<TiFile>::iterator it = _nvFileList.begin();
	int size = sizeof(TiFile);

	for (; it != _nvFileList.end(); it++)
	{
		if (!file.Write(&(*it), size))
			return false;
	}

	return true;
}

bool NveFile::save(const Tim::TString &file)
{
	_filePath = file;

	return save();
}

void NveFile::add(const TiFile &tiFile)
{
	_nvFileList.push_back(tiFile);
}

void NveFile::erase(size_t i)
{
	size_t j = 0;
	for (list<TiFile>::iterator it = _nvFileList.begin();
		it != _nvFileList.end(); it++, j++)
	{
		if (j == i)
		{
			_nvFileList.erase(it);
			break;
		}
	}
}

bool NveFile::empty()
{
	return _nvFileList.empty();
}

void NveFile::clear()
{
	if (!_nvFileList.empty()){
		list<TiFile>().swap(_nvFileList);
	}
}

void NveFile::add(const std::list<TiFile> &fileList)
{
	list<TiFile>::const_iterator it = fileList.begin();

	for (; it != fileList.end(); it++){
		_nvFileList.push_back(*it);
	}
}