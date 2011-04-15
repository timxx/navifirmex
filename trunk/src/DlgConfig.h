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

#ifndef __CONDIG_DIALOG_H__
#define __CONDIG_DIALOG_H__

#include "Tim\dialog.h"
#include "ColorButton.h"
#include "Config.h"

class DlgConfig : public Tim::Dialog
{
public:
	DlgConfig()
		:_pConfig(0),
		_crOldLeft(0), _crOldRight(0)
	{}

protected:
	virtual BOOL CALLBACK runProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnInit();
	void OnCommand(int id);

	void chooseColor(COLORREF &cr);

private:
	ColorButton _cbLeft;
	ColorButton _cbRight;

	Config *_pConfig;

	COLORREF _crOldLeft;	//
	COLORREF _crOldRight;
};

#endif