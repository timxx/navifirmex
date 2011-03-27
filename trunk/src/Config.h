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

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <Windows.h>

#include "Tim/Rect.h"
#include "TinyXml/tinyxml.h"

using namespace Tim;

class Config
{
public:
	Config(const char *filePath);
	~Config(){
		if (_cfgDoc)
			delete _cfgDoc;
	}

	bool load();
	bool save();

	void setPos(const Point &pos){
		_pos = pos;
	}
	void setTaskMgrRect(const Rect &rc, BOOL fMaxed = FALSE){
		_rcTask = rc;
		_tgMaxed = fMaxed;
	}
	BOOL isTaskMgrMaxed() const{
		return _tgMaxed;
	}
	void setIndex(int index){
		_nIndex = index;
	}
	void setLastDir(const LPCSTR lpDir){
		strcpy(_lastDir, lpDir);
	}

	void getLastDir(LPSTR lpDir) const{
		strcpy(lpDir, _lastDir);
	}
	int getXPos()	const	{	return _pos.x;		}
	int getYPos()	const	{	return _pos.y;		}
	const Rect& getTaskRect() const	{	return _rcTask;	} 
	int getIndex()	const	{	return _nIndex;		}

protected:
	void makeDefault();
private:
	Point	_pos;
	int		_nIndex;
	char	_lastDir[MAX_PATH];
	Rect	_rcTask;
	BOOL	_tgMaxed;
	TiXmlDocument *_cfgDoc;
	char _filePath[MAX_PATH];
};
#endif