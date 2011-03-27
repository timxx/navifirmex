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

#ifndef __NM_MESSAGE_H__
#define __NM_MESSAGE_H__

#define NM_CODEDOWNLOAD		WM_USER	+ 3
#define NM_INIT				WM_USER	+ 4
#define NM_SETSERVER		WM_USER	+ 5
#define NM_GETOVERWRITE		WM_USER	+ 6
#define NM_SETOVERWRITE		WM_USER	+ 7
#define NM_GETPROMPT		WM_USER	+ 8
#define NM_SETPROMPT		WM_USER	+ 9
#define NM_GETTASKRECT		WM_USER + 10
#define NM_GETLASTDIR		WM_USER + 11
#define NM_SETLASTDIR		WM_USER + 12
#define NM_ABOUTCLOSE		WM_USER + 13

#define NM_URLDOWN			WM_USER	+ 14

#define NM_DOWNFINISH		WM_USER	+ 30
#define NM_DOWNPROGRESS		WM_USER + 31

#endif