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

#ifndef __CONFIRM_DLG_H__
#define __CONFIRM_DLG_H__

#pragma once

#include "tim\dialog.h"

using namespace Tim;

class DlgConfirm : public Dialog
{
public:
	bool getResult(bool& fPrompt)
	{
		fPrompt = _fPrompt;
		return _fYes;
	}
	//call before create dialog
	void setText(LPCTSTR lpText){
		_infoText = lpText;
	}

protected:
	virtual BOOL CALLBACK runProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	bool _fYes;
	bool _fPrompt;
	TString _infoText;
};

#endif