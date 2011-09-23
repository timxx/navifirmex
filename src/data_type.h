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

#ifndef __DATA_TYPE_H__
#define __DATA_TYPE_H__

#include <vector>
// #include <map>
#include "Tim/TString.h"

//NOKIA��Ʒ
struct Product
{
	Product()/*:id(0)*/{}
	Product(/*const long &i*/const Tim::TString &i, const Tim::TString &t, const Tim::TString &n)
		:id(i), type(t), name(n)
	{}
	//long		 id;	//<productID xsi:type="xsd:long">		���ڲ�ѯRelease
	Tim::TString id;	//long��ʾ��Χ�Ĳ������������xsd:long��LONGLONG����ʹ
	Tim::TString type;	//<productType xsi:type="xsd:string">	RM-XXX
	Tim::TString name;	//<marketName xsi:type="xsd:string">	Nokia XXX
	Tim::TString image;	//ͼƬ����ZIP URL
};

//�汾
struct Release
{
	Release()/*:id(0)*/{}
	Release(/*const long &i*/const Tim::TString &i, const Tim::TString &v)
		:id(i), ver(v)
	{}
	//long		 id;	//<releaseID xsi:type="xsd:long">		���ڲ�ѯVariants�б�
	Tim::TString id;
	Tim::TString ver;	//<version xsi:type="xsd:string">		�汾��
};

//std::map<long, Tim::TString> Release;

struct FileInfo
{
	FileInfo():size(0){}
	FileInfo(const Tim::TString &n, const Tim::TString &u, const long &s)
		:name(n), url(u), size(s)
	{}
	Tim::TString name;	//<fileName xsi:type="xsd:string">
	Tim::TString url;	//<downloadURL xsi:type="xsd:string">
	long		 size;	//<fileSize xsi:type="xsd:long">
};

struct Variant
{
	Variant(){}
	Variant(const Tim::TString &n, const Tim::TString &c)
		:name(n), code(c)
	{}
//	long		 id;		//variantID xsi:type="xsd:long">
	Tim::TString name;		//<name xsi:type="xsd:string">			CODE��
	Tim::TString code;		//<productCode xsi:type="xsd:string">	CODE
	std::vector<FileInfo> vFiles;
};

class GUIWnd;

struct GetImageProcParam
{
	GetImageProcParam()
		:pWnd(0)
	{}

	GUIWnd *pWnd;
	Tim::TString fileToSave;
	Tim::TString fileUrl;
};

//�����ļ���Ϣ
struct TiFile
{
	char	url[MAX_PATH];	//���صĵ�ַ
	char	file[MAX_PATH];	//����·��
	long	offset;			//ƫ�ƣ�����0ʱ�����ϴ����أ�Ϊ0ʱ��������
	long	size;			//�ܴ�С�����ܴ�����longȡֵ��Χ

	TiFile()
		:offset(0), size(0)
	{
		RtlSecureZeroMemory(url, MAX_PATH);
		RtlSecureZeroMemory(file, MAX_PATH);
	}

	void seturl(const std::string &u){
		lstrcpyA(url, u.c_str());
	}
	void setfile(const std::string &f){
		lstrcpyA(file, f.c_str());
	}
};

class MiniDownloader;

struct DownloadStatus
{
	long total;	//�ܹ�����
	long now;	//��������

	MiniDownloader *pdm;

	DownloadStatus()
		:total(0), now(0), pdm(0)
	{}

	DownloadStatus(long t, long n, MiniDownloader *pObj)
		:total(t), now(n), pdm(pObj)
	{}
};

struct Proxy
{
	// proxy type, value:0 ~ 3
	// 0: not use
	// 1: http
	// 2: socks v4
	// 3: socks v5
	int				type;
	Tim::TString	server;
	int				port;
	Tim::TString	usr;
	Tim::TString	pwd;

	Proxy()
		:type(0), port(0)
	{}
};

#endif	//__DATA_TYPE_H__