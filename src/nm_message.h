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

//ͨ��CODE����, lParam����CODE
#define NM_CODEDOWNLOAD		WM_USER	+ 3

//��ʼ��������
#define NM_INIT				WM_USER	+ 4
//��ȡ\���ø���
#define NM_GETOVERWRITE		WM_USER	+ 6
#define NM_SETOVERWRITE		WM_USER	+ 7
//��ȡ\���ø�������
#define NM_GETPROMPT		WM_USER	+ 8
#define NM_SETPROMPT		WM_USER	+ 9
//�������񴰿�λ��
//wParam: �����Ƿ����, lParam:RECT
#define NM_GETTASKRECT		WM_USER + 10
//���һ��ѡ���Ŀ¼
#define NM_GETLASTDIR		WM_USER + 11
#define NM_SETLASTDIR		WM_USER + 12
//����׼���رյ�����������ʱ����
#define NM_ABOUTCLOSE		WM_USER + 13
//�½�URL����
#define NM_URLDOWN			WM_USER	+ 14
//��ϵͳ��������ʾ���ؽ���
#define NM_TASKBAR			WM_USER + 15
//һ���߳̽���, lParam�������
#define NM_ENDTHREAD		WM_USER	+ 16
//�������
#define NM_DOWNFINISH		WM_USER	+ 30
//���ؽ���
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