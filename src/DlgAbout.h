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
#ifndef ABOUT_DLG_H
#define ABOUT_DLG_H

#pragma once

#include "Tim/dialog.h"
#include "Tim/LinkCtrl.h"

using namespace _TIM;

class DlgAbout : public Dialog
{
public:
	DlgAbout(){}
	~DlgAbout(){}

protected:
	virtual BOOL CALLBACK runProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	LinkCtrl _qqLink;
	LinkCtrl _emailLink;
};

#endif
