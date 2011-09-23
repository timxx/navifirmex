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
//////////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////////
#ifndef	__GUI_WINDOW_H__
#define __GUI_WINDOW_H__
//////////////////////////////////////////////////////////////////////////
#include <list>
#include <shobjidl.h>
#include <GdiPlus.h>

#include "Tim\Dialog.h"
#include "Tim\CommonCtrls.h"
#include "res\resource.h"
#include "data_type.h"
#include "Http.h"
#include "TinyXml/tinyxml.h"
#include "ProductList.h"
#include "VariantList.h"
#include "Config.h"
#include "TaskMgrWnd.h"
#include "NveFile.h"
#include "DlgNewTask.h"
#include "DlgConfirm.h"
#include "LangHelper.h"
#include "StatusEx.h"
//////////////////////////////////////////////////////////////////////////
using namespace _TIM;
using namespace std;
//////////////////////////////////////////////////////////////////////////
class GUIWnd : public Window
{
public:
	GUIWnd():
		_hThreadProduct(INVALID_HANDLE_VALUE), _hThreadRelease(INVALID_HANDLE_VALUE),
		_hThreadVariant(INVALID_HANDLE_VALUE), _hTreadGetImage(INVALID_HANDLE_VALUE),
		_hbmpPhone(NULL), _hTreadGetFileList(INVALID_HANDLE_VALUE),
		_pConfig(0), _sortIndex(0),
		_fPrompt(TRUE), _fOverwrite(TRUE), _nvFile(NULL)
	{
		_taskMgr = NULL;
		_pTaskbar = NULL;
		_fDownloading = false;
		WM_TASKBARBUTTONCREATED = 0;
		_fDisableTaskbar = TRUE;
		_hbmpBkgnd = NULL;
		_newTaskDlg = NULL;
		_hFontChild = NULL;
		_curLangIndex = -1;
	}
	~GUIWnd(){}

	friend class LangHelper;

	virtual void init(HINSTANCE hinst, HWND hwndParent);
protected:
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnCommand(int id, HWND hwndCtl, UINT uNotifyCode);
	void OnNotify(int id, NMHDR *pnmh);
	void OnPaint();
	void OnClose();
	void OnDestroy();
	void OnAboutClose(BOOL bNotEmpty, std::list<TiFile> &fileList);
	void OnUrlDown(LPCTSTR url);
	void OnInitMenu(HMENU hMenu);
	void OnSize(int nWidth, int nHeight);

	void doProductChange();
	void doReleaseChange();
	void doVariantChange();
	void doProductFilter();
	void doVariantFilter();
	void doCopyUrl(UINT cmd);	//复制指定文件URL到剪切板
	void doDownLoad();
	void doSelAll(bool fYes = true);
	void doRefresh(bool fNeedACK = true);			//刷新

	//获取产品列表
	static DWORD WINAPI GetProductListProc(LPVOID lParam);
	//分析返回数据
	int ReadProduct(const char *data, DWORD dwSize);
	//读取一个产品
	void GetProductInfo(TiXmlNode *node);

	//Release列表
	static DWORD WINAPI GetReleaseListProc(LPVOID lParam);
	int	ReadRelease(const char *data, DWORD dwSize, const TString &productID);
	void GetReleaseInfo(TiXmlNode *node);

	//variant列表
	static DWORD WINAPI GetVariantListProc(LPVOID lParam);
	int	ReadVariant(const char *data, DWORD dwSize, const TString &releaseID);
	void GetVariantInfo(TiXmlNode *node, TiXmlNode *bodyNode);

	//终止线程
	void EndThread(HANDLE &hThread);

	void ShowProducts();
	void ShowReleases();
	void ShowVariants();

	void PopupFileListMenu();

	static bool SaveToFile(LPCTSTR lpPath, LPVOID data, DWORD dwLen);

	void ShowPhonePicture(const Product &curProduct);

	HBITMAP ImageFileToHBitmap(const TString &file);

	//程序根目录
	TString GetRootFolder();
	//程序根目录+subPath
	TString MakeFilePath(const TString &subPath);

	void RefreshProducts();

	static DWORD WINAPI GetImageProc(LPVOID lParam);
	void DownloadImage(const TString &url, const TString &fileSave);

	static DWORD GetFileData(const TString &filePath, LPVOID buffer);

	template<typename T>
	T * list_at(list<T> &listSrc, size_t index)
	{
		list<T>::iterator it = listSrc.begin();
		
		for (size_t i = 0; it != listSrc.end(); ++it, ++i)
		{
			if (i == index)
				return &(*it);
		}

		return NULL;
	}
	//根据文件的存在、大小判断是否需更新
	static bool FileNeedUpdate(const TString &filePath, DWORD dwMinFileSize = 0);

	//取#ID_XX的URL
	string GetFileUrlByID(TiXmlNode *bodyNode, const char *ID);

	void ExportProductToText();
	void ExportProductToXml();
	void ExportVariantToText();
	void ExportVariantToXml();

	TString SelectFilePath(const TString &defName);

	//取得指定CODE的文件列表
	static DWORD WINAPI GetFileListByCodeProc(LPVOID lParam);
	TString GetSessionID();
	void VariantDataToFiles(const char *data);

	TString GetFileElement(TiXmlNode *fileNode, const char *tagName, TiXmlNode *bodyNode = NULL);
	void ShowFiles();

	//判断返回数据是否为错误信息
	bool IsErrorInfo(TiXmlDocument *pDoc, TString &strErrInfo);
	//取得node第一个childElement的文本值
	TString GetChildElementText(TiXmlNode *node, const char *childElement);

	bool SaveExportText(LPCTSTR lpDefFileName, const TString &strData);
	void UpdateStatus(LPCSTR type, const TString &infoText, int nPart = 1);
	void ShowServer(int index = 1);

	void SortListItem(int index);

	//解压ZIP中name文件并返回内容指针， 数据大小存于dwSize
	static char*	GetFileDataFromZip(const TString& zipFile, const TString& name, DWORD& dwSize);
	//保存到ZIP
	static bool		SaveFileToZip(const TString& zipFile, const TString& name, LPVOID lpData, DWORD dwSize);
	//判断ZIP里是否有name文件
	static bool		FileExistsZip(const TString& zipFile, const TString& name); 
	char* GetSoap(Http& http, LPCTSTR lpPost, LPDWORD pdwSize);
	//尝试获取SESSIONID
	TString TryGetSessionID();
	//取得字符串资源
	TString LoadSoapString(UINT uID);

	//当前联网进度
	static void WebProgress(double total, double now, void* pGuiWnd);

	//如果下载任务窗口没创建则创建
	void ValidTaskMgr();
	void InitTaskBar();
	//是否显示滚动进度
	void ShowIndeterminateProgress(bool fShow = true);
	void ShowToolTip(bool fShow = true);
	//刷新Static控件背景
	void InvalidStatic(Static &staic);

//	void ExcludeChildRect(HDC hdc);

	void InitBackground();

	void LoadSession();

	void CreateControls();

	void LoadLanguage();
	bool IsLangFile(const TString &file);
	void InitLangMenu();

	bool SetLanguage(size_t i);
	void SetConfirmLang(DlgConfirm &dlg, const char *type);

	//resize the labels so it fits different language
	void ResizeLabels();

	bool PrepareLang(LangHelper &lang);

	int msgBox(LPCSTR type, const TString &text, const TString &caption = TEXT("MessageBox"), UINT uType = MB_OK);

	void setChildrenFont(const HFONT &hFont);

	void setProxy(Http &http);
private:
	TaskMgrWnd * _taskMgr;
	DlgNewTask * _newTaskDlg;

	Static	_labelProducts;
	Static	_labelProductsCount;

	Static	_labelReleases;
	Static	_labelReleasesCount;

	Static	_labelVariants;
	Static	_labelVariantsCount;

	Static	_labelFiles;
	Static	_labelFilesCount;

	Static	_picFrame;

	ProductList	_lbProduct;
	ListBox		_lbRelease;
	VariantList	_lbVariant;
	ListView	_lvFile;

	StatusEx	_status;

	Edit		_edProduct;
	Edit		_edVariant;

	Button		_btnDownload;

	list<Product>		_lProduct;
	vector<Release>		_vRelease;
	list<Variant>		_lVariant;

	list<Product>		_lProductFilter;
	list<Variant>		_lVariantFilter;

	TString	_jsessionidStr;

	HANDLE	_hThreadProduct;
	HANDLE	_hThreadRelease;
	HANDLE	_hThreadVariant;
	HANDLE	_hTreadGetImage;
	HANDLE	_hTreadGetFileList;

	HBITMAP	_hbmpPhone;

	GetImageProcParam _getImgProcParam;

	//通过CODE下载的CODE
	TString		_code;
	//显示的文件列表
	vector<FileInfo> _vFiles;

	//服务器列表
	static char *_szServer[3];

	Config	*_pConfig;

	int		_sortIndex;	//0, 1 only
	enum sort_type{SORT_UP, SORT_DOWN};
	sort_type	_sortType;	//

	//是否提示覆盖文件
	BOOL	_fPrompt;
	//用户选择覆盖还是跳过
	BOOL	_fOverwrite;

	NveFile *_nvFile;

	//在系统任务栏显示进度
	ITaskbarList3 *_pTaskbar;
	//当运行在非WIN7时禁用
	BOOL _fDisableTaskbar;
	//只用于设置任务栏进度时判断
	bool _fDownloading;
	//
	UINT WM_TASKBARBUTTONCREATED;

	HBITMAP	_hbmpBkgnd;

	HFONT	_hFontChild;

	vector<TString> _vLang;
	int _curLangIndex;	// current language index of _vLang

public:
	bool isCNSystem;	// whether current system is Chinese
};

#endif