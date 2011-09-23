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
#include <string>

#include "Tim/Rect.h"
#include "TinyXml/tinyxml.h"
#include "data_type.h"

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

	void setGUIRect(const Rect &rc, BOOL fMaxed = FALSE)	{
		_rcGUI = rc;
		_fGUIMaxed = fMaxed;
	}
	void setTaskMgrRect(const Rect &rc, BOOL fMaxed = FALSE){
		_rcTask = rc;
		_tgMaxed = fMaxed;
	}
	void setIndex(int index)	{	_nIndex = index;	}
	void setLastDir(const LPCSTR lpDir)	{	strcpy(_lastDir, lpDir);	}
	void setDownPrompt(BOOL fPrompt)	{	_fDownloadPrompt = fPrompt;	}
	void setDownAction(BOOL fYes)		{	_fDownAction = fYes;		}
	void setExitPrompt(BOOL fPrompt)	{	_fExitPrompt = fPrompt;		}
	void setExitAction(BOOL fYes)		{	_fExitAction = fYes;		}
	void setColor(COLORREF cr1, COLORREF cr2)	{	_cr1 = cr1; _cr2 = cr2;	}
	void setShowTaskMgr(BOOL fShow)	{	_fShowTaskMgr = fShow;	}
	void setLangFile(const std::string &file)	{	_langFile = file;	}
	
	void getLastDir(LPSTR lpDir) const	{	strcpy(lpDir, _lastDir);	}
	const Rect &getGUIRect()	const	{	return _rcGUI;				}
	BOOL isGUIMaxed() const	{	return _fGUIMaxed;	}
	const Rect& getTaskRect() const	{	return _rcTask;	}
	BOOL isTaskMgrMaxed() const	{	return _tgMaxed;	}
	int  getIndex()	const	{	return _nIndex;			}
	void getColor(COLORREF &cr1, COLORREF &cr2)	{	cr1 = _cr1; cr2 = _cr2;	}
	BOOL downloadWithPompt()const	{	return _fDownloadPrompt;	}
	BOOL downloadAction()	const	{	return _fDownAction;		}
	BOOL exitWithPrompt()	const	{	return _fExitPrompt;		}
	BOOL extiAction()		const	{	return _fExitAction;		}
	BOOL showTaskMgr()		const	{	return _fShowTaskMgr;		}

	std::string getLangFile()const	{	return _langFile;			}

	void setFont(const LOGFONT &lf, COLORREF cr)	{	_lFont = lf;	_crFont = cr;	}
	const LOGFONT& getFont() const					{	return _lFont;					}
	COLORREF getFontColor() const					{	return _crFont;					}

	void setProxy(const Proxy &proxy)	{	_proxy = proxy;	}
	void getProxy(Proxy &proxy)	const	{	proxy = _proxy;	}

	bool useProxy() const {	return _proxy.type > 0 && _proxy.type < 4; }
protected:
	void makeDefault();

	COLORREF makeColor(const char *color);
	char *makeColor(COLORREF cr);

	inline int HexToDec(char hex);

	void loadWindow(TiXmlNode *node);
	void loadDownload(TiXmlNode *node);

	void loadFont(TiXmlNode *node);

	void writeWindow(TiXmlNode *node);
	void writeDownload(TiXmlNode *node);

	void writeFont(TiXmlNode *node);

	void loadProxy(TiXmlNode *node);
	void writeProxy(TiXmlNode *node);

private:
	Rect	_rcGUI;
	BOOL	_fGUIMaxed;
	int		_nIndex;
	char	_lastDir[MAX_PATH];
	Rect	_rcTask;
	BOOL	_tgMaxed;
	TiXmlDocument *_cfgDoc;
	char _filePath[MAX_PATH];
	//主窗口渐变颜色
	COLORREF _cr1;
	COLORREF _cr2;

	BOOL	_fDownloadPrompt;	//运行时是否提示接着下载
	BOOL	_fDownAction;		//TRUE - 点击确定
	BOOL	_fExitPrompt;		//退出有任务时是否提醒
	BOOL	_fExitAction;		//TRUE － 确定
	BOOL	_fShowTaskMgr;

	LOGFONT		_lFont;
	COLORREF	_crFont;

	Proxy	_proxy;

	std::string	_langFile;
};
#endif