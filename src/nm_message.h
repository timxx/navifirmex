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

//通过CODE下载, lParam传送CODE
#define NM_CODEDOWNLOAD		WM_USER	+ 3

//初始化主窗口
#define NM_INIT				WM_USER	+ 4
//获取\设置覆盖
#define NM_GETOVERWRITE		WM_USER	+ 6
#define NM_SETOVERWRITE		WM_USER	+ 7
//获取\设置覆盖提醒
#define NM_GETPROMPT		WM_USER	+ 8
#define NM_SETPROMPT		WM_USER	+ 9
//下载任务窗口位置
//wParam: 窗口是否最大化, lParam:RECT
#define NM_GETTASKRECT		WM_USER + 10
//最后一次选择的目录
#define NM_GETLASTDIR		WM_USER + 11
#define NM_SETLASTDIR		WM_USER + 12
//程序准备关闭但有下载任务时发送
#define NM_ABOUTCLOSE		WM_USER + 13
//新建URL下载
#define NM_URLDOWN			WM_USER	+ 14
//在系统任务栏显示下载进度
#define NM_TASKBAR			WM_USER + 15
//一个线程结束, lParam表明句柄
#define NM_ENDTHREAD		WM_USER	+ 16
//下载完成
#define NM_DOWNFINISH		WM_USER	+ 30
//下载进度
#define NM_DOWNPROGRESS		WM_USER + 31

#define NM_SHOWTASKMGR		WM_USER	+ 32

#define NM_CHANGEUI			WM_USER	+ 40

//wParam for hwnd, lParam for "type"
#define NM_SETDIALOGLANG	WM_USER	+ 41

//wParam: hmenu, lParam: "type"
#define NM_SETPOPMENULANG	WM_USER	+ 42

//lparam: TCHAR *
#define NM_GETLANGPATH		WM_USER	+ 43

// lparam: vector<TString> *
#define NM_GETPROXYTYPE		WM_USER	+ 44

// lparam: MiniDownloader *
#define NM_SETPROXY			WM_USER + 45
#endif