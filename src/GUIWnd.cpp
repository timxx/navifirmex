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
#include <algorithm>
#include <shlwapi.h>
#include <map>

#include "GUIWnd.h"
#include "Tim/ClassEx.h"
#include "Zip/unzip.h"
#include "Zip/zip.h"
#include "DlgAbout.h"
#include "DlgNew.h"

#include "curl/curl.h"
#include "nm_message.h"
#include "Tim/File.h"
#include "common.h"
#include "DlgConfirm.h"
#include "DlgNewTask.h"
#include "DlgConfig.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Msimg32.lib")

#pragma warning(disable:4018)
//////////////////////////////////////////////////////////////////////////
#define AGENT_NAME		"NaviFirmEx/1.3"

#define UPDATE_TIMER_ID	2

#define ASSERT_SERVER_INDEX(i)	if(i > 3 || i < 0) i = 1
//////////////////////////////////////////////////////////////////////////
char *GUIWnd::_szServer[3] = 
{
	"https://www.caresuite.nokia.com/caresuite/CareSuite?WSDL",
	"https://www.dsut.online.nokia.com/oti/CareSuite?WSDL",
	"https://www.dsut-qa.online.nokia.com/oti/CareSuite?WSDL"
};
//////////////////////////////////////////////////////////////////////////
//记录当前文件列表排序是升还是降
bool gfsortUp = true;
//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK GUIWnd::runProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		OnCommand(LOWORD(wParam), HWND(lParam), HIWORD(wParam));
		break;

	case WM_NOTIFY:
		OnNotify(int(wParam), (NMHDR*)lParam);
		break;

	case WM_PAINT:
		OnPaint();
		break;

	case WM_INITDIALOG:
		//OnInit();
		//解决程序运行时窗口有时透明的问题
		PostMessage(_hWnd, NM_INIT, 0, 0);
		return TRUE;

	case WM_MENUSELECT:
		{
			UINT id[] = {0, IDR_MENU_MAIN};
			::MenuHelp(uMsg, wParam, lParam, GetMenu(), _hinst, _status.getSelf(), id);
		}
		break;

	case NM_INIT:
		OnInit();
		break;

	case WM_INITMENU:
		OnInitMenu(HMENU(wParam));
		break;

	case WM_CLOSE:
		OnClose();
		break;

	case WM_TIMER:
		if (wParam == UPDATE_TIMER_ID)
		{
			UpdateStatus(TEXT(""));
			KillTimer(UPDATE_TIMER_ID);
		}
		break;

	case NM_CODEDOWNLOAD:
		{
			TCHAR *code = (TCHAR*)lParam;
			if (!code)
				break;

			_code = code;

			EndThread(_hTreadGetFileList);
			vector<FileInfo>().swap(_vFiles);
			_lvFile.DeleteAllItems();
			//Mar. 20, 2011
			//修正通过CODE下载时没更新显示
			SetItemText(IDS_FILE_LIST, TEXT("(0)"));
			_hTreadGetFileList = CreateThread(0, 0, GetFileListByCodeProc, this, 0, NULL);
			if (_hTreadGetFileList == INVALID_HANDLE_VALUE){
				msgBox(TEXT("创建线程时出错了！"), TEXT("错误"), MB_ICONERROR);
			}else{
				_imgProcess->Play();
			}

			delete [] code;
		}
		break;

	case NM_GETOVERWRITE:
		//return _fOverwrite;
		(*(BOOL*)lParam) = _fOverwrite;
		break;

	case NM_SETOVERWRITE:
		_fOverwrite = wParam;
		break;

	case NM_GETPROMPT:
		//return _fPrompt;
		(*(BOOL*)lParam) = _fPrompt;
		break;

	case NM_SETPROMPT:
		_fPrompt = wParam;
		break;

	case NM_GETTASKRECT:
		if (_pConfig)
		{
			(*(Rect*)lParam) = _pConfig->getTaskRect();
			(*(BOOL*)wParam) = _pConfig->isTaskMgrMaxed();
		}
		break;

	case NM_GETLASTDIR:
		if (_pConfig){
			_pConfig->getLastDir((char *)lParam);
		}else{
			((char *)lParam)[0] = 0;
		}
		break;

	case NM_SETLASTDIR:
		if (_pConfig){
			_pConfig->setLastDir((char*)lParam);
		}
		break;

	case WM_DESTROY:
		OnDestroy();
		break;

	case NM_ABOUTCLOSE:
		OnAboutClose(wParam, *((list<TiFile>*)lParam));
		break;

	case NM_URLDOWN:
		OnUrlDown((TCHAR*)lParam);
		delete [] (TCHAR*)lParam;
		break;

	case NM_TASKBAR:
		{
			if (_pTaskbar)
			{				
				if (wParam == 100)
				{
					_pTaskbar->SetProgressState(_hWnd, TBPF_NOPROGRESS);
					_fDownloading = false;
				}
				else
				{
					_fDownloading = true;
					_pTaskbar->SetProgressValue(_hWnd, wParam, 100);
				}
			}
		}
		break;

	case NM_ENDTHREAD:
		{
			HANDLE *hThread = (HANDLE*)lParam;
			if (hThread &&((*hThread) != INVALID_HANDLE_VALUE))
			{
				CloseHandle(*hThread);
				*hThread = INVALID_HANDLE_VALUE;
			}
			ShowIndeterminateProgress(false);
		}
		break;

	case NM_DOWNFINISH:
		sendMsg(NM_TASKBAR, 100);	//防止任务管理获取的进度不准的情况
		KillTimer(UPDATE_TIMER_ID);
		UpdateStatus(TEXT("全部下载任务已完成"));
		SetTimer(UPDATE_TIMER_ID, 5000);
		break;

	case WM_CTLCOLORSTATIC:
		SetBkMode(HDC(wParam), TRANSPARENT);
		return (BOOL)GetStockObject(NULL_BRUSH);

	case NM_CHANGEUI:
		InitBackground();
		break;

	case NM_SHOWTASKMGR:
		if (_pConfig)
		{
			if (_pConfig->showTaskMgr())
				(*(BOOL*)lParam) = TRUE;
			else
				(*(BOOL*)lParam) = FALSE;
		}
		break;

	default:
		if (uMsg == WM_TASKBARBUTTONCREATED){
			InitTaskBar();
		}
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::OnCommand(int id, HWND hwndCtl, UINT uNotifyCode)
{
	switch(id)
	{
	case IDC_LIST_PRODUCT:	//改变型号
		if (uNotifyCode == LBN_SELCHANGE)
			doProductChange();
		break;

	case IDC_LIST_RELEASE:	//版本改变
		if (uNotifyCode == LBN_SELCHANGE)
			doReleaseChange();
		break;

	case IDC_LIST_VARIANT:	//Variant改变
		if (uNotifyCode == LBN_SELCHANGE)
			doVariantChange();
		break;

	case IDE_PRODUCT_FILTER:	//型号过滤
		if (uNotifyCode == EN_CHANGE)
			doProductFilter();
		break;

	case IDE_VARIANT_FILTER:	//variant过滤
		if (uNotifyCode == EN_CHANGE)
			doVariantFilter();
		break;

	case IDM_SEL_ALL:	//全选/取消全选
	case IDM_SEL_NULL:
		doSelAll(id == IDM_SEL_ALL);
		break;

	case IDM_COPY_CURRENT:	//复制文件URL
	case IDM_COPY_SELECTION:
	case IDM_COPY_ALL:
		doCopyUrl(id);
		break;

	case IDB_DOWNLOAD:
		doDownLoad();
		break;

	case IDM_REFRESH:
		doRefresh();
		break;

	case IDM_ABOUT:
		{
			DlgAbout aboutDlg;
			aboutDlg.init(_hinst, _hWnd);
			aboutDlg.doModal(IDD_ABOUT);
		}
		break;

	case IDM_EXPORT_PRODUCT_TEXT:
		ExportProductToText();
		break;

	case IDM_EXPORT_PRODUCT_XML:
		ExportProductToXml();
		break;

	case IDM_EXPORT_VARIANT_TEXT:
		ExportVariantToText();
		break;

	case IDM_EXPORT_VARIANT_XML:
		ExportVariantToXml();
		break;

	case IDM_EXIT:
		sendMsg(WM_CLOSE);
		break;

	case IDM_NEW:
		{
			DlgNew newDlg;
			newDlg.init(_hinst, _hWnd);
			newDlg.doModal(IDD_NEW_DOWN);
		}
		break;

	case IDM_SERVER_NOKIA:
	case IDM_SERVER_EXTERNAL:
	case IDM_SERVER_QA:
		{
			int index = id - IDM_SERVER_NOKIA;
			
			if (_pConfig)
			{
				if (_pConfig->getIndex() != index)
					doRefresh(false);
			}

			_pConfig->setIndex(index);

			ShowServer(index);
		}
		break;

	case IDM_HELP:
		{
			TString helpFile = MakeFilePath(TEXT("Help.chm"));
			if (File::Exists(helpFile))
			{
				ShellExecute(_hWnd, TEXT("open"), helpFile, NULL, NULL, SW_SHOW);
			}
			else
			{
				msgBox(TEXT("详细请登陆塞班论坛，链接：\r\n")
					TEXT("http://bbs.dospy.com/thread-10732732-1-341-1.html\r\n")
					TEXT("（可按下Ctrl+C复制信息）"),
					TEXT("帮助"),
					MB_ICONINFORMATION
					);
			}
		}
		break;

	case IDM_TASK_WND:
		ValidTaskMgr();
		if (!_taskMgr->IsWindowVisible()){
			_taskMgr->showWindow();
		}
		break;

	case IDM_DOWN_CONTINUE:
		if (_nvFile && !_nvFile->empty())
		{
			ValidTaskMgr();

			list<TiFile> vlist = _nvFile->getlist();
			list<TiFile>::iterator it = vlist.begin();

			for ( ; it != vlist.end(); it++){
				_taskMgr->newTask(*it);
			}
		}
		else
		{
			msgBox(TEXT("无文件可下载！"), TEXT("继续下载"), MB_ICONINFORMATION);
		}

		{
			HMENU hMenu = GetMenu();
			hMenu = GetSubMenu(hMenu, 0);
			RemoveMenu(hMenu, 2, MF_BYPOSITION);
			RemoveMenu(hMenu, 2, MF_BYPOSITION);
		}

		break;

	case IDM_SETTINGS:
		{
			DlgConfig dlgCfg;
			dlgCfg.init(getHinst(), getSelf());
			dlgCfg.doModal(IDD_CONFIG, _pConfig);
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::OnNotify(int id, NMHDR *pnmh)
{
	switch(pnmh->code)
	{
	case NM_RCLICK:
		ShowPopupMenu();
		break;

	case LVN_ITEMCHANGED:
		for (size_t i=0; i<_lvFile.GetItemCount(); i++)
		{
			if (_lvFile.GetCheck(i))//有打勾时下载按钮才可用
			{
				_btnDownload.enable();
				return ;
			}
		}
		_btnDownload.disable();
		break;

	case LVN_COLUMNCLICK:	//单击文件列表视图头
		{
			int whichItem = ((NMLISTVIEW *)pnmh)->iSubItem;
			if (whichItem > 2)
				break;

			SortListItem(whichItem);
			ShowFiles();
		}
		break;

	case LVN_HOTTRACK:
		{
			LPNMLISTVIEW lpnmlv = (LPNMLISTVIEW)pnmh;
			int index = lpnmlv->iItem;
			if (index < 0 || index > _vFiles.size())
				break;

			TString strInfo;
			strInfo.format(TEXT("名称：%s\r\n大小：%s\r\nURL： %s"),
				_vFiles[index].name.c_str(),					//name
				MakeFileSizeFmt(_vFiles[index].size).c_str(),	//size
				_vFiles[index].url.c_str());					//url

			LVSETINFOTIP lvSetInfoTip = {0};
			lvSetInfoTip.iItem = index;
			lvSetInfoTip.cbSize = sizeof(LVSETINFOTIP);
#if !defined UNICODE
			lvSetInfoTip.pszText = (wchar_t*)atow(strInfo).c_str();
#else
			lvSetInfoTip.pszText = (wchar_t*)strInfo.c_str();
#endif
			_lvFile.SetInfoTip(&lvSetInfoTip);
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::OnPaint()
{
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(_hWnd, &ps);

	//Fixed bug when restore window child controls
	//will be covered by background
	//Apr. 13, 2011
	ExcludeChildRect(hdc);

	if (_hbmpBkgnd)
	{
		Rect rect;
		GetClientRect(&rect);

		DrawBitmap(_hbmpBkgnd, hdc, rect);
	}

	if (_hbmpPhone) //
	{
		HDC hdcMem = CreateCompatibleDC(hdc);
		HGDIOBJ hOldObj = SelectObject(hdcMem, _hbmpPhone);

		Rect rect;
		::GetWindowRect(HwndFromId(IDS_FRAME), &rect);
		ScreenToClient(&rect);

		BITMAP bmp = {0};
		GetObject(_hbmpPhone, sizeof(BITMAP), &bmp);

		int destWidth = rect.Width();
		int destHeight = rect.Height();

		if (bmp.bmWidth != bmp.bmHeight)
		{
			if (bmp.bmWidth > bmp.bmHeight){
				destHeight = bmp.bmHeight;
			}else{
				destWidth = bmp.bmWidth;
			}
		}

		TransparentBlt(hdc, rect.left, rect.top, destWidth, destHeight,
			hdcMem, 0, 0, bmp.bmWidth, bmp.bmHeight, RGB(255, 255, 255));

		SelectObject(hdcMem, hOldObj);
		DeleteDC(hdcMem);
	}

	EndPaint(_hWnd, &ps);
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::OnInit()
{
	InitCommonControls();
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	Gdiplus::GdiplusStartup(&_gdiplusToken, &gdiplusStartupInput, NULL);

	OSVERSIONINFO ovi = {0};
	ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&ovi))
	{
		//Win7 => v6.1
		if (ovi.dwMajorVersion >= 6 &&
			ovi.dwMinorVersion >= 1
			)
		{
			WM_TASKBARBUTTONCREATED = RegisterWindowMessage(TEXT("TaskbarButtonCreated"));
			_fDisableTaskbar = FALSE;
		}
	}

	SetDlgIcon(IDI_MAIN);

	TString cfgPath = MakeFilePath(TEXT("\\Config.xml"));
	if (!cfgPath.empty())
	{	
#ifdef UNICODE
		int len = wtoa(cfgPath.c_str(), NULL);
		char *apath = new char[len];
		wtoa(cfgPath.c_str(), apath, len);
		_pConfig = new Config(apath);
		delete [] apath;
#else
		_pConfig = new Config(cfgPath.c_str());
#endif
	}
	else
	{
		_pConfig = new Config("Config.xml");
	}

	//还原最后记录窗口位置
	if (_pConfig->load()){
		moveTo(_pConfig->getXPos(), _pConfig->getYPos());
	}

	InitBackground();

	_status.init(_hinst, _hWnd);
	_status.create();

	_edProduct.assign(IDE_PRODUCT_FILTER, _hWnd);
	_edVariant.assign(IDE_VARIANT_FILTER, _hWnd);

	_lbProduct.assign(IDC_LIST_PRODUCT, _hWnd);
	_lbRelease.assign(IDC_LIST_RELEASE, _hWnd);
	_lbVariant.assign(IDC_LIST_VARIANT, _hWnd);
	_lvFile.assign(IDC_LIST_FILES, _hWnd);

	_btnDownload.assign(IDB_DOWNLOAD, _hWnd);

	_lvFile.SetExtendedStyle(LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );

	LVCOLUMN lvColumn = {0};

	lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;

	lvColumn.pszText = TEXT("文件名");
	lvColumn.cx = 150;
	_lvFile.InsertColumn(0, &lvColumn);

	lvColumn.pszText = TEXT("大小");
	lvColumn.cx = 70;
	_lvFile.InsertColumn(1, &lvColumn);

	//_lvFile.SetHoverTime(10);

	_imgProcess = new Gdiplus::ImageEx(_hinst, MAKEINTRESOURCE(IDR_GIF_PROCESS), TEXT("GIF"));

	int nParts[4] = {32, _status.getWidth()*5/7, _status.getWidth()*5/7 + 75, -1};
	_status.SetParts(4, nParts);
	ShowServer(_pConfig->getIndex());

	Rect rect;
	_status.GetRect(0, &rect);

	int x = rect.left + (rect.Width() - 16)/2;
	int y = rect.top + (rect.Height() - 16)/2;

	_imgProcess->InitAnimation(_status.getSelf(), x, y);

	 _hThreadProduct = CreateThread(0, 0, GetProductListProc, this, 0, 0);

 	if (_hThreadProduct == INVALID_HANDLE_VALUE)
 	{
		_imgProcess->Pause();

		UpdateStatus(TEXT("创建线程出错了，获取产品列表失败……"));

 		msgBox(TEXT("GUIWnd::OnInit: 创建线程出错了"), TEXT("Error"), MB_ICONERROR);
 	}

	TString nve = MakeFilePath(TEXT("task.nve"));

	if (File::Exists(nve))
	{
		_nvFile = new NveFile;

		if (_nvFile->load(nve))
		{
			BOOL fYes = TRUE;
			if (_pConfig)
			{
				if (_pConfig->downloadWithPompt())
				{
					DlgConfirm confirm;

					confirm.init(getHinst(), getSelf());
					confirm.setTitle(TEXT("继续下载"));
					confirm.setText(TEXT("是否继续下载上次未完成的任务？"));

					confirm.doModal(IDD_CONFIRM);
					bool fPrompt;
					fYes = confirm.getResult(fPrompt);
					_pConfig->setDownPrompt(fPrompt);
					_pConfig->setDownAction(fYes);
				}
			}

			if (fYes)
			{
				ValidTaskMgr();

				if (_pConfig)
				{
					if (_pConfig->showTaskMgr())
						_taskMgr->showWindow();
				}
				else
				{
					_taskMgr->showWindow();
				}

				list<TiFile> vlist = _nvFile->getlist();
				list<TiFile>::iterator it = vlist.begin();

				for ( ; it != vlist.end(); it++){
					_taskMgr->newTask(*it);
				}
			}
			else
			{
				HMENU hMenu = GetMenu();
				hMenu = GetSubMenu(hMenu, 0);
				InsertMenu(hMenu, 2, MF_BYPOSITION | MF_BYCOMMAND, IDM_DOWN_CONTINUE, TEXT("继续下载(&I)"));
				InsertMenu(hMenu, 3, MF_BYPOSITION | MF_SEPARATOR, 0, 0);
			}
		}

		File::Delete(nve);
	}
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::OnClose()
{
	if (_taskMgr)
	{
		if (_taskMgr->hasTask())
		{
			if (_pConfig)
			{
				if (_pConfig->exitWithPrompt())
				{
					DlgConfirm confirm;

					confirm.init(getHinst(), getSelf());
					confirm.setTitle(TEXT("关闭程序"));
					confirm.setText(TEXT("还有下载任务在进行，确定要中止下载并退出程序吗？"));

					confirm.doModal(IDD_CONFIRM);
					bool fPrompt;
					bool fYes = confirm.getResult(fPrompt);
					_pConfig->setExitPrompt(fPrompt);
					_pConfig->setExitAction(fYes);
					if (!fYes){
						return ;
					}
				}
			}
		}

		::SendMessage(_taskMgr->getSelf(), NM_ABOUTCLOSE, 0, 0);
	}
	else
	{
		sendMsg(WM_DESTROY);
	}
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::OnDestroy()
{
	if (_imgProcess){
		delete _imgProcess;
	}

	EndThread(_hThreadProduct);
	EndThread(_hThreadRelease);
	EndThread(_hThreadVariant);
	EndThread(_hTreadGetFileList);
	EndThread(_hTreadGetImage);

	if (_hbmpPhone){
		DeleteObject(_hbmpPhone);
	}

	if (_pConfig)
	{
		Rect rect;
		GetWindowRect(&rect);

		_pConfig->setPos(Point(rect.left, rect.top));

		if (_taskMgr)
		{
			if (!_taskMgr->isMaximized())
			{
				_taskMgr->GetWindowRect(&rect);
				_pConfig->setTaskMgrRect(rect, FALSE);
			}
			else
			{
				_pConfig->setTaskMgrRect(_pConfig->getTaskRect(), TRUE);
			}
		}

		_pConfig->save();

		delete _pConfig;
	}

	if (_taskMgr){
		delete _taskMgr;
	}
	if (_newTaskDlg){
		delete _newTaskDlg;
	}

	if (_nvFile)
	{
		if (!_nvFile->empty())
		{
			TString nve = MakeFilePath(TEXT("task.nve"));
			_nvFile->save(nve);	//确保能正确保存
		}

		delete _nvFile;
	}

	if (_gdiplusToken){
		Gdiplus::GdiplusShutdown(_gdiplusToken);
	}

	if (_pTaskbar){
		_pTaskbar->Release();
	}

	if (!_fDisableTaskbar){
		CoUninitialize();
	}

	if (_hbmpBkgnd){
		DeleteObject(_hbmpBkgnd);
	}

	PostQuitMessage(0);
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::OnAboutClose(BOOL bNotEmpty, std::list<TiFile> &fileList)
{
	if (_nvFile){
		_nvFile->clear();	//之前的要释放掉
	}else{
		_nvFile = new NveFile;
	}

	if (bNotEmpty){
		_nvFile->add(fileList);
	}

	sendMsg(WM_DESTROY);
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::OnUrlDown(LPCTSTR url)
{
	if (!url){
		return ;
	}

	FileInfo file;

	file.size = 0;
	file.url = url;

#ifdef UNICODE
	wstring name = File::GetFileName(url);
	if (name.empty()){
		name = url;
	}
	file.name = name.c_str();
#else
	wstring name = File::GetFileName(atow(url));
	if (name.empty()){
		file.name = url;
	}else{
		file.name = wtoa(name).c_str();
	}
#endif

	ValidTaskMgr();

	_taskMgr->newTask(file);
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::OnInitMenu(HMENU hMenu)
{
	UINT cmd[3] = {IDM_SERVER_NOKIA, IDM_SERVER_EXTERNAL, IDM_SERVER_QA};
	int i = 1;
	if (_pConfig){
		i = _pConfig->getIndex();
	}

	ASSERT_SERVER_INDEX(i);

	CheckMenuRadioItem(hMenu, cmd[0], cmd[2], cmd[i], MF_CHECKED);

}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::doProductChange()
{
	if (_lProductFilter.empty())
		return ;

	int sel = _lbProduct.GetCurSel();
	if (sel < 0 || sel >= _lProductFilter.size())
		return ;

	_lbRelease.ResetContent();
	_lbVariant.ResetContent();
	_lvFile.DeleteAllItems();

	//清空并释放空间
	vector<Release>().swap(_vRelease);
	list<Variant>().swap(_lVariant);
	list<Variant>().swap(_lVariantFilter);
	vector<FileInfo>().swap(_vFiles);

	ShowReleases();
	ShowVariants();
	ShowFiles();

	Product *product = list_at(_lProductFilter, sel);
	ShowPhonePicture(*product);

	EndThread(_hThreadRelease);

	_hThreadRelease = CreateThread(0, 0, GetReleaseListProc, this, 0, 0);
	if (_hThreadRelease == INVALID_HANDLE_VALUE)
	{
		DWORD dwErr = GetLastError();
		TString info;
		info.format(TEXT("GUIWnd::doProductChange: 创建线程出错了[%d]"), dwErr);
		msgBox(info, TEXT("错误"), MB_ICONERROR);
	}
	else
	{
		_imgProcess->Play();
	}
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::doReleaseChange()
{
	if (_vRelease.empty())
		return;

	int sel = _lbRelease.GetCurSel();
	if (sel < 0 || sel >= _vRelease.size())
		return ;

	_lbVariant.ResetContent();
	_lvFile.DeleteAllItems();

	list<Variant>().swap(_lVariant);
	list<Variant>().swap(_lVariantFilter);
	vector<FileInfo>().swap(_vFiles);

	ShowVariants();
	ShowFiles();

	EndThread(_hThreadVariant);

	_hThreadVariant = CreateThread(0, 0, GetVariantListProc, this, 0, 0);

	if (_hThreadVariant == INVALID_HANDLE_VALUE){
		msgBox(TEXT("GUIWnd::doReleaseChange: 创建线程出错了"), TEXT("Error"), MB_ICONERROR);
	}else{
		_imgProcess->Play();
	}
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::doVariantChange()
{
	if (_lVariantFilter.empty())
		return ;

	int sel = _lbVariant.GetCurSel();
	if (sel < 0 || sel >= _lVariantFilter.size())
		return ;

	_lvFile.DeleteAllItems();

	Variant *variant = list_at(_lVariantFilter, sel);

	vector<FileInfo>().swap(_vFiles);

	_vFiles = variant->vFiles;

	HDITEM hdi = {0};
	hdi.mask = HDI_FORMAT;
	hdi.fmt = HDF_STRING;
	HWND hLvHdr = _lvFile.GetHeader();
	if (hLvHdr)
	{
		Header_SetItem(hLvHdr, 0, &hdi);
		Header_SetItem(hLvHdr, 1, &hdi);
	}

	ShowFiles();
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::doProductFilter(){
	ShowProducts();
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::doVariantFilter(){
	ShowVariants();
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::doCopyUrl(UINT cmd)
{
	TString urlStr;

	if (cmd == IDM_COPY_CURRENT)	//复制当前选中的文件
	{
		int sel = _lvFile.GetSelectionMark();
		if (sel >0 && sel < _vFiles.size())
			urlStr = _vFiles[sel].url;
	}
	else if (cmd == IDM_COPY_SELECTION)	//打勾
	{
		for (size_t i=0; i<_lvFile.GetItemCount(); i++)
		{
			if (_lvFile.GetCheck(i)){
				urlStr += _vFiles[i].url + TEXT("\r\n");
			}
		}
	}
	else	//全部
	{
		for (size_t i=0; i<_lvFile.GetItemCount(); i++){
			urlStr += _vFiles[i].url + TEXT("\r\n");
		}
	}

	if (!urlStr.empty())
	{
		if (!CopyTextToClipbrd(urlStr.c_str()))
			msgBox(TEXT("复制URL到剪贴板失败！"), TEXT("复制URL"), MB_ICONERROR);
		else
			UpdateStatus(TEXT("成功复制URL到系统剪贴板"));
	}
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::doDownLoad()
{
	ValidTaskMgr();

	_newTaskDlg->showWindow();

	for (size_t i=0; i<_lvFile.GetItemCount(); i++)
	{
		if (_lvFile.GetCheck(i)){
			_newTaskDlg->AddTask(_vFiles[i]);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::doSelAll(bool fYes /* = true */)
{
	for (size_t i=0; i<_lvFile.GetItemCount(); i++)
		_lvFile.SetCheck(i, fYes);
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::doRefresh(bool fNeedACK/* = true*/)
{
	if (fNeedACK)
	{
		if (
			msgBox(TEXT("确定要刷新产品吗？这将清空所有缓存文件！"),
			TEXT("请确认"), MB_ICONQUESTION | MB_YESNO) == IDNO
			)
			return ;
	}

	_lbProduct.ResetContent();
	_lbRelease.ResetContent();
	_lbVariant.ResetContent();
	_lvFile.DeleteAllItems();

	if (_hbmpPhone)
	{
		DeleteObject(_hbmpPhone);
		_hbmpPhone = NULL;
		InvalidateRect(NULL, TRUE);
	}

	TString().swap(_jsessionidStr);

	::SendDlgItemMessage(_hWnd, IDS_PRODUCT, WM_SETTEXT, 0, (LPARAM)TEXT("(0/0)"));
	::SendDlgItemMessage(_hWnd, IDS_RELEASE, WM_SETTEXT, 0, (LPARAM)TEXT("(0)"));
	::SendDlgItemMessage(_hWnd, IDS_VARIANT, WM_SETTEXT, 0, (LPARAM)TEXT("(0/0)"));
	::SendDlgItemMessage(_hWnd, IDS_FILE_LIST, WM_SETTEXT, 0, (LPARAM)TEXT("(0)"));

	//fixed: Apr. 12, 2011
	InvalidStatic(IDS_PRODUCT);
	InvalidStatic(IDS_RELEASE);
	InvalidStatic(IDS_VARIANT);
	InvalidStatic(IDS_FILE_LIST);

	list<Product>().swap(_lProduct);
	list<Product>().swap(_lProductFilter);
	
	vector<Release>().swap(_vRelease);

	list<Variant>().swap(_lVariant);
	list<Variant>().swap(_lVariantFilter);
	vector<FileInfo>().swap(_vFiles);

	RefreshProducts();

	EndThread(_hThreadProduct);
	EndThread(_hThreadRelease);
	EndThread(_hThreadVariant);
	EndThread(_hTreadGetImage);

	_hThreadProduct = CreateThread(0, 0, GetProductListProc, this, NULL, NULL);

	if (_hThreadProduct == INVALID_HANDLE_VALUE){
		msgBox(TEXT("无法创建获取产品线程！！"), TEXT("出错了"), MB_ICONERROR);
	}else{
		_imgProcess->Play();
	}
}
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI GUIWnd::GetProductListProc(LPVOID lParam)
{
	GUIWnd *pWnd = (GUIWnd*)lParam;

	if (!pWnd)
	{
		MessageBox(0, TEXT("GUIWnd::GetProductListProc: 无效传递"), TEXT("Error"), MB_ICONERROR);
		return 0;
	}

	pWnd->KillTimer(UPDATE_TIMER_ID);
	pWnd->ShowIndeterminateProgress();

	//86024CAD1E83101D97359D7351051156 => products MD5

	TString productFile = pWnd->MakeFilePath(TEXT("Cache\\86024CAD1E83101D97359D7351051156"));
	char *data = NULL;
	bool fNeedClear = false;
	DWORD dwSize = 0;
	Http http;

	if (FileNeedUpdate(productFile, 10*1024))
	{
back:
		TString postData = pWnd->LoadSoapString(IDS_PRODUCT_SOAP);
		if (postData.empty())
			goto _exit;

		TString session = pWnd->TryGetSessionID();
		if (session.empty())
			goto _exit;

		postData.Replace(TEXT("[sID]"), session);

		data = pWnd->GetSoap(http, postData, &dwSize);
	}
	else
	{
		pWnd->UpdateStatus(TEXT("正在读取缓存文件……"));

		data = GetFileDataFromZip(productFile, TEXT("products.xml"), dwSize);
		if (!data)
			goto back;

		fNeedClear = true;
	}

	pWnd->ReadProduct(data, dwSize);

	if (fNeedClear && data)
		delete [] data;

	pWnd->ShowProducts();

_exit:
	if (pWnd->_lProduct.empty()){
		pWnd->_lbProduct.AddString(TEXT("<空>"));
	}
	pWnd->_imgProcess->Pause();
	pWnd->UpdateStatus(TEXT("结束获取产品列表……"));
	pWnd->SetTimer(UPDATE_TIMER_ID, 3000);

	PostMessage(pWnd->getSelf(), NM_ENDTHREAD, 0, (LPARAM)(&(pWnd->_hThreadProduct)));

	return 1;
}
//////////////////////////////////////////////////////////////////////////
int GUIWnd::ReadProduct(const char *data, DWORD dwSize)
{
	if (!data)
	{
		UpdateStatus(TEXT("数据无效，获取产品列表失败……"));
		return 0;
	}

	int ret = 0;

	TiXmlDocument *pXmlDoc = new TiXmlDocument;
	pXmlDoc->Parse(data);

	TString cacheFolder = MakeFilePath(TEXT("Cache"));
	TString errInfo;

	if (pXmlDoc->Error())
	{
		UpdateStatus(TEXT("TinyXml::Parse数据出错"));
		goto _exit;
	}

	if (IsErrorInfo(pXmlDoc, errInfo))
	{
		UpdateStatus(TEXT("获取产品列表失败"));

		TString str(TEXT("获取产品列表时发生错误，服务器返回信息：\r\n"));
		str += errInfo;
		msgBox(str, TEXT("获取产品列表失败"), MB_ICONERROR);

		goto _exit;
	}

	//不存在Cache文件夹
	if (!PathFileExists(cacheFolder)){
		CreateDirectory(cacheFolder, NULL);
	}
	cacheFolder += TEXT("\\86024CAD1E83101D97359D7351051156");
	if (FileNeedUpdate(cacheFolder)){
		SaveFileToZip(cacheFolder, TEXT("products.xml"), (LPVOID)data, dwSize);
	}

	char *tagName[3] = {"env:Body", "m:GetProductListResponse", "result"};

	TiXmlNode *nodeResult = pXmlDoc->FirstChild("env:Envelope");
	if (!nodeResult)
		goto _exit;

	for (size_t i=0; i<3; i++)
	{
		nodeResult = nodeResult->FirstChild(tagName[i]);
		if (!nodeResult)
			goto _exit;
	}

	TiXmlNode *nodeProduct = nodeResult->FirstChild("product");

	UpdateStatus(TEXT("正在分析返回数据……"));

	for (; nodeProduct; nodeProduct = nodeProduct->NextSibling()){
		GetProductInfo(nodeProduct);
	}

	ret = 1;
_exit:

	
	if (pXmlDoc)
		delete pXmlDoc;

	return ret;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::GetProductInfo(TiXmlNode *node)
{
	if (!node){
		return ;
	}

	Product product;

	TString value = GetChildElementText(node, "productID");
	if (!value.empty()){
		product.id = value;
	}else{
		return ;
	}

	product.type = GetChildElementText(node, "productType");
	product.name = GetChildElementText(node, "marketName");

	node = node->FirstChildElement("files");

	if (node)
	{
		node = node->FirstChild("file");
		//寻找有.zip的URL
		for (; node; node = node->NextSibling())
		{
			value = GetChildElementText(node, "fileName");
			
			if (value.empty())
				continue;

			if (value.find(TEXT(".zip"), 0, false))
			{
				product.image = GetChildElementText(node, "downloadURL");
				if (!product.image.empty())
					break;
			}
		}
	}

	//不能两者皆空
	if (!product.type.empty() && !product.name.empty()){
		_lProduct.push_back(product);
	}
}
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI GUIWnd::GetReleaseListProc(LPVOID lParam)
{
	GUIWnd *pWnd = (GUIWnd*)lParam;
	int index = pWnd->_lbProduct.GetCurSel();

	Product *product = pWnd->list_at(pWnd->_lProductFilter, index);
	TString productID = product->id;

	pWnd->KillTimer(UPDATE_TIMER_ID);
	pWnd->ShowIndeterminateProgress();

	if (pWnd->_hTreadGetImage != INVALID_HANDLE_VALUE)
	{
		pWnd->UpdateStatus(TEXT("等待图片下载……"));

		WaitForSingleObject(pWnd->_hTreadGetImage, 5000);
		CloseHandle(pWnd->_hTreadGetImage);
		pWnd->_hTreadGetImage = INVALID_HANDLE_VALUE;
	}

	char *data = NULL;
	bool fNeedClear = false;

	//02B67C3EAE678DC49209D6DE4709A171 => release
	TString rFile = pWnd->MakeFilePath(TEXT("Cache\\02B67C3EAE678DC49209D6DE4709A171"));

	DWORD dwSize = 0;
	Http http;

	if (!FileExistsZip(rFile, productID))
	{
back:
		TString postData = pWnd->LoadSoapString(IDS_RELEASE_SOAP);
		if (postData.empty())
			goto _exit;

		TString session = pWnd->TryGetSessionID();
		if (session.empty())
			goto _exit;

		postData.Replace(TEXT("[sID]"),session);
		postData.Replace(TEXT("[pID]"), productID);

		data = pWnd->GetSoap(http, postData, &dwSize);
	}
	else
	{
		data = GetFileDataFromZip(rFile, productID, dwSize);
		if (!data)
			goto back;

		fNeedClear = true;

	}

	pWnd->ReadRelease(data, dwSize, productID);

	pWnd->ShowReleases();

_exit:
	if (pWnd->_vRelease.empty()){
		pWnd->_lbRelease.AddString(TEXT("<空>"));
	}

	pWnd->_imgProcess->Pause();

	if (fNeedClear && data)
		delete [] data;

	pWnd->UpdateStatus(TEXT("结束获取发行版本列表……"));

	pWnd->SetTimer(UPDATE_TIMER_ID, 5000);

	PostMessage(pWnd->getSelf(), NM_ENDTHREAD, 0, reinterpret_cast<LPARAM>(&(pWnd->_hThreadRelease)));

	return 1;
}
//////////////////////////////////////////////////////////////////////////
int GUIWnd::ReadRelease(const char *data, DWORD dwSize, const TString &productID)
{
	if (!data)
	{
		UpdateStatus(TEXT("无效数据，读取发行版本数据失败……"));
		return 0;
	}

	int ret = 0;

	TiXmlDocument *pXmlDoc = new TiXmlDocument;
	pXmlDoc->Parse(data);

	TString folder = MakeFilePath(TEXT("Cache"));
	TString errInfo;

	if (pXmlDoc->Error())
	{
		UpdateStatus(TEXT("TinyXml::Parse数据出错"));
		goto _exit;
	}

	if (IsErrorInfo(pXmlDoc, errInfo))
	{
		UpdateStatus(TEXT("获取发行版本列表失败"));
		TString str(TEXT("获取发行版本列表时发生错误，服务器返回信息：\r\n"));
		str += errInfo;
		msgBox(str, TEXT("获取发行版本列表失败"), MB_ICONERROR);

		goto _exit;
	}


	if (!PathFileExists(folder.c_str()))
		CreateDirectory(folder.c_str(), NULL);

	folder += TEXT("\\02B67C3EAE678DC49209D6DE4709A171");
	if (!FileExistsZip(folder, productID))
		SaveFileToZip(folder, productID, (LPVOID)data, dwSize);

	TiXmlNode *nodeResult = pXmlDoc->FirstChild("env:Envelope");
	if (!nodeResult)
		goto _exit;

	char *tagName[3] = {"env:Body", "m:GetReleasesForProductResponse", "result"};

	for (size_t i=0; i<3; i++)
	{
		nodeResult = nodeResult->FirstChild(tagName[i]);
		if (!nodeResult)
			goto _exit;
	}

	TiXmlNode *nodeRelease = nodeResult->FirstChild("release");

	UpdateStatus(TEXT("正在分析返回数据……"));

	for (; nodeRelease; nodeRelease = nodeRelease->NextSibling()){
		GetReleaseInfo(nodeRelease);
	}

	ret = 1;

_exit:
	if (pXmlDoc)
		delete pXmlDoc;

	return ret;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::GetReleaseInfo(TiXmlNode *node)
{
	if (!node){
		return ;
	}

	Release verInfo;

	verInfo.id = GetChildElementText(node, "releaseID");
	if (verInfo.id.empty())
		return ;

	verInfo.ver = GetChildElementText(node, "version");

	if (!verInfo.ver.empty())
		_vRelease.push_back(verInfo);
}
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI GUIWnd::GetVariantListProc(LPVOID lParam)
{
	GUIWnd *pWnd = (GUIWnd*)lParam;

	int index = pWnd->_lbRelease.GetCurSel();
	pWnd->ShowIndeterminateProgress();

	TString releaseID = pWnd->_vRelease[index].id;

	//796E0E5FF8114B13B2E23CDE9D3C6904 -> variants
	TString vFile = pWnd->MakeFilePath(TEXT("Cache\\796E0E5FF8114B13B2E23CDE9D3C6904"));

	Http http;
	char *data = NULL;
	bool fNeedClear = false;
	DWORD dwSize = 0;
	pWnd->KillTimer(UPDATE_TIMER_ID);

	if (!FileExistsZip(vFile, releaseID))
	{
back:
		TString postData = pWnd->LoadSoapString(IDS_VARIANT_SOAP);
		if (postData.empty())
			goto _exit;

		TString session = pWnd->TryGetSessionID();
		if (session.empty())
			goto _exit;

		postData.Replace(TEXT("[sID]"), session);
		postData.Replace(TEXT("[vID]"), releaseID);

		//postData += postData;

		data = pWnd->GetSoap(http, postData, &dwSize);
	}
	else
	{
		data = GetFileDataFromZip(vFile, releaseID, dwSize);
		if (!data){
			goto back;
		}

		fNeedClear = true;
	}

	pWnd->ReadVariant(data, dwSize, releaseID);

	pWnd->ShowVariants();

_exit:
	if (pWnd->_lVariant.empty()){
		pWnd->_lbVariant.AddString(TEXT("<空>"));
	}

	pWnd->_imgProcess->Pause();

	if (fNeedClear && data)
		delete [] data;

	pWnd->UpdateStatus(TEXT("结束获取CODE列表……"));
	pWnd->SetTimer(UPDATE_TIMER_ID, 5000);

	PostMessage(pWnd->getSelf(), NM_ENDTHREAD, 0, reinterpret_cast<LPARAM>(&(pWnd->_hThreadVariant)));

	return 1;
}
//////////////////////////////////////////////////////////////////////////
int GUIWnd::ReadVariant(const char *data, DWORD dwSize, const TString &releaseID)
{
	if (!data)
	{
		UpdateStatus(TEXT("无效数据，读取CODE列表数据失败……"));
		return 0;
	}

	int ret = 0;
	TString folder = MakeFilePath(TEXT("Cache"));

	TiXmlDocument *pXmlDoc = new TiXmlDocument;
	pXmlDoc->Parse(data);
	TString errInfo;

	if (pXmlDoc->Error())
	{
		goto _exit;
	}

	if (IsErrorInfo(pXmlDoc, errInfo))
	{
		UpdateStatus(TEXT("获取CODE列表失败"));

		TString str(TEXT("获取CODE列表时发生错误，服务器返回信息：\r\n"));
		str += errInfo;
		msgBox(str, TEXT("获取CODE列表失败"), MB_ICONERROR);

		goto _exit;
	}

	if (!PathFileExists(folder.c_str()))
		CreateDirectory(folder.c_str(), NULL);

	folder += TEXT("\\796E0E5FF8114B13B2E23CDE9D3C6904");
	if (!FileExistsZip(folder, releaseID))
		SaveFileToZip(folder, releaseID, (LPVOID)data, dwSize);

	TiXmlNode *nodeResult = pXmlDoc->FirstChild("env:Envelope");
	if (!nodeResult)
		goto _exit;

	char *tagName[3] = {"env:Body", "m:GetReleaseVariantsResponse", "result"};
	TiXmlNode *bodyNode = NULL;
	for (size_t i=0; i<3; i++)
	{
		nodeResult = nodeResult->FirstChild(tagName[i]);
		if (!nodeResult)
			goto _exit;
		if (i == 0)
			bodyNode = nodeResult;
	}

	TiXmlNode *nodeVariant = nodeResult->FirstChild("variant");

	UpdateStatus(TEXT("正在分析返回数据……"));

	for (; nodeVariant; nodeVariant = nodeVariant->NextSibling()){
		GetVariantInfo(nodeVariant, bodyNode);
	}

	ret = 1;
_exit:

	if (pXmlDoc)
		delete pXmlDoc;

	return ret;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::GetVariantInfo(TiXmlNode *node, TiXmlNode *bodyNode)
{
	if (!node){
		return ;
	}

	Variant variant;

	variant.name = GetChildElementText(node, "name");
	variant.code = GetChildElementText(node, "productCode");

	TiXmlNode *nodeFile = node->FirstChild("files");
	if (!nodeFile){
		return ;
	}

	nodeFile = nodeFile->FirstChild("file");

	FileInfo fileInfo;

	for (; nodeFile; nodeFile = nodeFile->NextSibling())
	{
		fileInfo.name = GetFileElement(nodeFile, "fileName");
		if (fileInfo.name.empty())
			continue;

		TString sizeStr = GetFileElement(nodeFile, "fileSize");
		if (!sizeStr.empty()){
			fileInfo.size = _ttol(sizeStr.c_str());
		}

		fileInfo.url = GetFileElement(nodeFile, "downloadURL", bodyNode);
		if (fileInfo.url.empty())
			continue;

		variant.vFiles.push_back(fileInfo);
	}//end for

	_lVariant.push_back(variant);
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::EndThread(HANDLE &hThread)
{
	if(hThread != INVALID_HANDLE_VALUE)
	{
		TerminateThread(hThread, 0);
		CloseHandle(hThread);

		hThread = INVALID_HANDLE_VALUE;
	}
}
//////////////////////////////////////////////////////////////////////////
bool product_cmp(const Product &lhs, const Product &rhs)
{
	TString lstr = lhs.name;
	lstr += TEXT(" (") + lhs.type + TEXT(")");

	TString rstr = rhs.name;
	rstr += TEXT(" (") + rhs.type + TEXT(")");

	return lstrcmpi(lstr.c_str(), rstr.c_str()) < 0;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ShowProducts()
{
	TString tmpStr;
	
	_lProduct.sort(product_cmp);

	_lbProduct.ResetContent();

	list<Product>().swap(_lProductFilter);

	if (_edProduct.empty())
	{
		_lProductFilter = _lProduct;
	}
	else
	{
		TString filterStr = _edProduct.getText();
		filterStr.trim();

		for (list<Product>::iterator it = _lProduct.begin();
			it != _lProduct.end(); ++it)
		{
			if (it->name.find(filterStr, 0, false)!=TString::npos ||
				it->type.find(filterStr, 0, false)!=TString::npos
				)
			{
				_lProductFilter.push_back(*it);
			}
		}

	}
	HDC hdc = GetDC(_hWnd);
	SIZE size = {0};

	for (list<Product>::iterator it = _lProductFilter.begin();
		it != _lProductFilter.end(); ++it)
	{
		tmpStr = it->name;
		tmpStr += TEXT(" (") + it->type + TEXT(")");

		GetTextExtentPoint32(hdc, tmpStr.c_str(), tmpStr.length(), &size); 
		if (size.cx > _lbProduct.GetHorizontalExtent())
			_lbProduct.SetHorizontalExtent(size.cx);

		_lbProduct.AddString(tmpStr);
	}

	ReleaseDC(_hWnd, hdc);

	tmpStr.format(TEXT("(%d/%d)"), _lProductFilter.size(), _lProduct.size());
	SendItemMsg(IDS_PRODUCT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(tmpStr.c_str()));

	InvalidStatic(IDS_PRODUCT);
}
//////////////////////////////////////////////////////////////////////////
bool release_cmp(const Release &lhs, const Release &rhs){
	return lhs.ver < rhs.ver;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ShowReleases()
{
	if (!_vRelease.empty())
		sort(_vRelease.begin(), _vRelease.end(), release_cmp);

	_lbRelease.ResetContent();

	for (size_t i=0; i<_vRelease.size(); i++)
	{
		_lbRelease.AddString(_vRelease[i].ver);
	}

	TString tmpStr;
	tmpStr.format(TEXT("(%d)"), _vRelease.size());
	SendItemMsg(IDS_RELEASE, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(tmpStr.c_str()));

	InvalidStatic(IDS_RELEASE);
}
//////////////////////////////////////////////////////////////////////////
bool variant_cmp(const Variant &lhs, const Variant &rhs)
{
	TString lStr = lhs.name;
	lStr += TEXT(" (") + lhs.code + TEXT(")");

	TString rStr = rhs.name;
	rStr += TEXT(" (") + rhs.code + TEXT(")");

	return lstrcmpi(lStr.c_str(), rStr.c_str()) < 0;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ShowVariants()
{
	if (!_lVariant.empty())
		_lVariant.sort(variant_cmp);

	list<Variant>().swap(_lVariantFilter);

	_lbVariant.ResetContent();

	if (_edVariant.empty())
	{
		_lVariantFilter = _lVariant;
	}
	else
	{
		TString filterStr = _edVariant.getText();
		filterStr.trim();
		for (list<Variant>::iterator it=_lVariant.begin();
			it != _lVariant.end(); ++it)
		{
			if (it->name.find(filterStr, 0, false)!=TString::npos ||
				it->code.find(filterStr, 0, false)!=TString::npos
				)
			{
				_lVariantFilter.push_back(*it);
			}
		}
	}

	HDC hdc = GetDC(_hWnd);
	SIZE size = {0};
	TString tmpStr;

	for (list<Variant>::iterator it=_lVariantFilter.begin();
		it != _lVariantFilter.end(); ++it)
	{
		tmpStr = it->name;
		tmpStr += TEXT(" (") + it->code + TEXT(")");

		_lbVariant.AddString(tmpStr);

		GetTextExtentPoint32(hdc, tmpStr.c_str(), tmpStr.length(), &size); 
		if (size.cx > _lbVariant.GetHorizontalExtent())
			_lbVariant.SetHorizontalExtent(size.cx);
	}

	ReleaseDC(_hWnd, hdc);

	tmpStr.format(TEXT("(%d/%d)"), _lVariantFilter.size(), _lVariant.size());
	SendItemMsg(IDS_VARIANT, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(tmpStr.c_str()));

	InvalidStatic(IDS_VARIANT);
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ShowPopupMenu()
{
	if (_lvFile.GetItemCount() == 0 )
		return ;

	HMENU hMenu = LoadMenu(IDR_MENU_RKEY);
	if (!hMenu)	return ;

	HMENU hMenuTP = ::GetSubMenu(hMenu, 0);
	if (!hMenuTP)	return ;

	bool bFlag = false;

	for (size_t i=0; i<_lvFile.GetItemCount(); i++)
	{
		if (_lvFile.GetCheck(i))
		{
			bFlag = true;
			break;
		}
	}

	if (!bFlag){
		::EnableMenuItem(hMenuTP, IDM_COPY_SELECTION, MF_BYCOMMAND | MF_DISABLED);
	}

	POINT pt;
	::GetCursorPos(&pt);
	::TrackPopupMenuEx(hMenuTP, TPM_BOTTOMALIGN, pt.x, pt.y, _hWnd, 0);

	::DestroyMenu(hMenu);
}
//////////////////////////////////////////////////////////////////////////
bool GUIWnd::SaveToFile(LPCTSTR lpPath, LPVOID data, DWORD dwLen)
{
	HANDLE hFile = CreateFile(lpPath, GENERIC_WRITE,  0,\
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (!hFile){
		return false;
	}

	DWORD dwWritten = 0;

	BOOL fStatus = WriteFile(hFile, data, dwLen, &dwWritten, NULL);

	CloseHandle(hFile);

	return fStatus == TRUE;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ShowPhonePicture(const Product &curProduct)
{
	bool fNeedGetImage = false;

	TString image = MakeFilePath(TEXT("Cache"));
	if (!PathFileExists(image.c_str()))
	{
		CreateDirectory(image.c_str(), NULL);
		fNeedGetImage = true;
	}
	
	image += TEXT("\\Image");

	if (!PathFileExists(image.c_str()))
	{
		CreateDirectory(image.c_str(), NULL);

		fNeedGetImage = true;
	}

	image += TEXT("\\") + curProduct.type + TEXT(".png");

	if (!fNeedGetImage)
	{
		if (!PathFileExists(image.c_str()))
			fNeedGetImage = true;
	}

	Rect rect;
	::GetWindowRect(HwndFromId(IDS_FRAME), &rect);
	ScreenToClient(&rect);

	if (_hbmpPhone)
	{
		DeleteObject(_hbmpPhone);
		_hbmpPhone = NULL;

		InvalidateRect(&rect, TRUE);
	}

	if (fNeedGetImage)
	{
		EndThread(_hTreadGetImage);

		_getImgProcParam.pWnd = this;
		_getImgProcParam.fileToSave = image;
		_getImgProcParam.fileUrl = curProduct.image;

		_hTreadGetImage = CreateThread(NULL, 0, GetImageProc, (LPVOID)&_getImgProcParam, NULL, NULL);
	}
	else
	{
		_hbmpPhone = ImageFileToHBitmap(image);

		InvalidateRect(&rect, TRUE);
	}
}
//////////////////////////////////////////////////////////////////////////
HBITMAP GUIWnd::ImageFileToHBitmap(const TString &file)
{
#ifdef UNICODE
	Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap(file.c_str());
#else
	int len = atow(file.c_str(), NULL);
	wchar_t *wfile = new wchar_t[len];
	atow(file.c_str(), wfile, len);

	Gdiplus::Bitmap *pBitmap = new Gdiplus::Bitmap(wfile);

	delete [] wfile;
#endif
	HBITMAP hBitmap = NULL;

	pBitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap);

	delete pBitmap;

	return hBitmap;
}
//////////////////////////////////////////////////////////////////////////
TString GUIWnd::GetRootFolder()
{
	TCHAR rootDir[MAX_PATH];

	GetModuleFileName(_hinst, rootDir, MAX_PATH);

	PathRemoveFileSpec(rootDir);

	int len = lstrlen(rootDir);
	if (rootDir[len-1] == TEXT('\\'))
		rootDir[len-1] = TEXT('\0');

	return rootDir;
}
//////////////////////////////////////////////////////////////////////////
TString GUIWnd::MakeFilePath(const TString &subPath)
{
	TString path = GetRootFolder();

	path += TEXT("\\") + subPath;

	return path;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::RefreshProducts()
{
	TString cacheFolder = MakeFilePath(TEXT("Cache"));

	if (!PathFileExists(cacheFolder.c_str()))
		return ;

	TString file = cacheFolder;
	file += TEXT("\\86024CAD1E83101D97359D7351051156");
	DeleteFile(file);

	file = cacheFolder;
	file += TEXT("\\796E0E5FF8114B13B2E23CDE9D3C6904");
	DeleteFile(file);

	file = cacheFolder;
	file += TEXT("\\02B67C3EAE678DC49209D6DE4709A171");
	DeleteFile(file);
}
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI GUIWnd::GetImageProc(LPVOID lParam)
{
	GetImageProcParam *pParam = (GetImageProcParam*)lParam;
	pParam->pWnd->DownloadImage(pParam->fileUrl, pParam->fileToSave);

	return 1;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::DownloadImage(const TString &url, const TString &fileSave)
{
	DWORD dwBufSize = 0;

	Http http;

	http.Init(false, AGENT_NAME);
	http.ShowProgress(WebProgress, this);
#ifdef UNICODE
 	http.Get(wtoa(url));
#else
 	http.Get(url);
 #endif
	char *buffer = http.GetData(&dwBufSize);
	if (!buffer || !dwBufSize)
		return ;

	ZIPENTRY ze = {0};

	HZIP hZip = OpenZip(buffer, dwBufSize, NULL);
	if (!hZip){
		return;
	}

	GetZipItem(hZip, -1, &ze);

	int totalIndex = ze.index;

	for (size_t i=0; i<totalIndex; i++)
	{
		GetZipItem(hZip, i, &ze);

		//只认第一张找到的！
		if (StrStrI(ze.name, TEXT(".png")) != 0)
		{
			if (UnzipItem(hZip, i, fileSave.c_str()) == ZR_OK)
			{
				_hbmpPhone = ImageFileToHBitmap(fileSave);

				Rect rect;
				::GetWindowRect(HwndFromId(IDS_FRAME), &rect);
				ScreenToClient(&rect);

				InvalidateRect(&rect, TRUE);
			}
			else
			{
				msgBox(TEXT("解压ZIP失败！"), TEXT("获取图片失败"), MB_ICONERROR);
			}

			break;
		}
	}
	if (hZip)
		CloseZip(hZip);
}
//////////////////////////////////////////////////////////////////////////
DWORD GUIWnd::GetFileData(const TString &filePath, LPVOID buffer)
{
	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ,
		FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if (!hFile)
		return 0;

	DWORD dwFileSize = GetFileSize(hFile, NULL);

	if (!buffer)
	{
		CloseHandle(hFile);
		return dwFileSize;
	}

	DWORD dwRead;

	ReadFile(hFile, buffer, dwFileSize, &dwRead, NULL);

	CloseHandle(hFile);

	return dwRead;
}
//////////////////////////////////////////////////////////////////////////
bool GUIWnd::FileNeedUpdate(const TString &filePath, DWORD dwMinFileSize/* = 0*/)
{
	HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, 
		FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return true;

	if (GetFileSize(hFile, NULL) <= dwMinFileSize)
	{
		CloseHandle(hFile);
		return true;
	}

	CloseHandle(hFile);

	return false;
}
//////////////////////////////////////////////////////////////////////////
string GUIWnd::GetFileUrlByID(TiXmlNode *bodyNode, const char *ID)
{
	if (!bodyNode || !ID)
		return "";

	// <xsd:string xsi:type="xsd:string" id="ID_2409">fileURL</xsd:string>

	TiXmlElement *element = bodyNode->FirstChildElement("xsd:string");

	for (; element; element = element->NextSiblingElement())
	{
		const char *id = element->Attribute("id");
		if (id)
		{
			if (StrCmpIA(id, ID) != 0)
				continue;

			const char *url = element->GetText();
			if (url){
				return url;
			}else{
				return "";
			}
		}
	}

	return "";
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ExportProductToText()
{
	TString productData;

	for (list<Product>::iterator it=_lProductFilter.begin();
		it != _lProductFilter.end(); ++it)
	{
		productData += it->type + TEXT("\t") + it->name + TEXT("\r\n");
	}

	if (!productData.empty())
	{
		if (!SaveExportText(TEXT("Products.txt"), productData))
			msgBox(TEXT("保存文件失败！！"), TEXT("导出文本"), MB_ICONERROR);
	}
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ExportProductToXml()
{
	if (_lProductFilter.empty())
		return ;

	TiXmlDocument *pXmlDoc = new TiXmlDocument;

	pXmlDoc->Parse("<ProductList/>");

	TiXmlNode *nodeProduct = pXmlDoc->FirstChild("ProductList");

	for (list<Product>::iterator it = _lProductFilter.begin();
		it != _lProductFilter.end(); ++it)
	{
		TiXmlElement element("product");
#ifdef UNICODE
		int len = wtoa(it->type.c_str(), NULL);
		char *buffer = new char[len];
		wtoa(it->type.c_str(), buffer, len);
		element.SetAttribute("type", buffer);

		delete [] buffer;

		len = wtoa(it->name.c_str(), NULL);
		buffer = new char[len];
		wtoa(it->name.c_str(), buffer, len);
		element.SetAttribute("name", buffer);

		delete [] buffer;

#else
		element.SetAttribute("type", it->type.c_str());
		element.SetAttribute("name", it->name.c_str());
#endif

		nodeProduct->InsertEndChild(element);
	}

	TString filePath = SelectFilePath(TEXT("Products.xml"));

	if (!filePath.empty())
	{
#ifdef UNICODE
		int len = wtoa(filePath.c_str(), NULL);
		char *afile = new char[len];
		wtoa(filePath.c_str(), afile, len);
		pXmlDoc->SaveFile(afile);
		delete [] afile;
#else
		pXmlDoc->SaveFile(filePath.c_str());
#endif
	}

	delete pXmlDoc;
}
//////////////////////////////////////////////////////////////////////////
bool variant_cmp_code(const Variant &lhs, const Variant &rhs)
{
	TString lStr = lhs.code;
	lStr += TEXT(" (") + lhs.name + TEXT(")");

	TString rStr = rhs.code;
	rStr += TEXT(" (") + rhs.name + TEXT(")");

	return lstrcmpi(lStr.c_str(), rStr.c_str()) < 0;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ExportVariantToText()
{
	if (_lVariantFilter.empty())
		return ;

	list<Variant> VariantsList = _lVariantFilter;

	VariantsList.sort(variant_cmp_code);

	TString productData;

	for (list<Variant>::iterator it=VariantsList.begin();
		it != VariantsList.end(); ++it)
	{
		productData += it->code + TEXT("\t") + it->name + TEXT("\r\n");
	}

	if (!productData.empty())
	{
		if (!SaveExportText(TEXT("Variants.txt"), productData))
			msgBox(TEXT("保存文件失败！！"), TEXT("导出文本"), MB_ICONERROR);
	}
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ExportVariantToXml()
{
	if (_lVariantFilter.empty())
		return ;

	list<Variant> VariantsList = _lVariantFilter;

	VariantsList.sort(variant_cmp_code);

	TiXmlDocument *pXmlDoc = new TiXmlDocument;

	pXmlDoc->Parse("<VariantList/>");

	TiXmlNode *nodeProduct = pXmlDoc->FirstChild("VariantList");

	for (list<Variant>::iterator it = VariantsList.begin();
		it != VariantsList.end(); ++it)
	{
		TiXmlElement element("variant");
#ifdef UNICODE
		int len = wtoa(it->code.c_str(), NULL);
		char *buffer = new char[len];
		wtoa(it->code.c_str(), buffer, len);
		element.SetAttribute("code", buffer);

		delete [] buffer;

		len = wtoa(it->name.c_str(), NULL);
		buffer = new char[len];
		wtoa(it->name.c_str(), buffer, len);
		element.SetAttribute("name", buffer);

		delete [] buffer;

#else
		element.SetAttribute("code", it->code.c_str());
		element.SetAttribute("name", it->name.c_str());
#endif

		nodeProduct->InsertEndChild(element);
	}

	TString filePath = SelectFilePath(TEXT("Variants.xml"));

	if (!filePath.empty())
	{
#ifdef UNICODE
		int len = wtoa(filePath.c_str(), NULL);
		char *afile = new char[len];
		wtoa(filePath.c_str(), afile, len);
		pXmlDoc->SaveFile(afile);
		delete [] afile;
#else
		pXmlDoc->SaveFile(filePath.c_str());
#endif
	}

	delete pXmlDoc;

}
//////////////////////////////////////////////////////////////////////////
TString GUIWnd::SelectFilePath(const TString &defName)
{
	OPENFILENAME ofn = {0};
	TCHAR filePath[MAX_PATH] = {0};

	lstrcpy(filePath, defName.c_str());

	ofn.hInstance 	= _hinst;
	ofn.hwndOwner 	= _hWnd;
	ofn.Flags 		= OFN_EXPLORER | OFN_OVERWRITEPROMPT;
	ofn.lpstrFile 	= filePath;
	ofn.nMaxFile 	= MAX_PATH;
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFilter = TEXT("All files(*.*)\0*.*\0\0");
	ofn.lpstrTitle 	= TEXT("保存文件");

	if (GetSaveFileName(&ofn))
		return filePath;

	return TEXT("");
}
//////////////////////////////////////////////////////////////////////////
DWORD WINAPI GUIWnd::GetFileListByCodeProc(LPVOID lParam)
{
	GUIWnd *pWnd = (GUIWnd*)lParam;

	if (!pWnd){
		MessageBox(0, TEXT("GUIWnd::GetVariantProc: 无效传递"), TEXT("Error"), MB_ICONERROR);
		return 0;
	}

	char *data = NULL;
	DWORD dwSize = 0;

	Http http;

	pWnd->KillTimer(UPDATE_TIMER_ID);
	pWnd->ShowIndeterminateProgress();

	TString session;
	TString postData = pWnd->LoadSoapString(IDS_CODE_SOAP);
	if (postData.empty())
		goto _exit;

	session = pWnd->TryGetSessionID();
	if (session.empty())
		goto _exit;

	postData.Replace(TEXT("[sID]"), session);
	postData.Replace(TEXT("[code]"), pWnd->_code);

	data = pWnd->GetSoap(http, postData, &dwSize);

	pWnd->VariantDataToFiles(data);

	pWnd->ShowFiles();

_exit:
	pWnd->_imgProcess->Pause();

	pWnd->UpdateStatus(TEXT("结束获取指定CODE文件列表线程……"));
	pWnd->SetTimer(UPDATE_TIMER_ID, 3000);

	PostMessage(pWnd->getSelf(), NM_ENDTHREAD, 0, reinterpret_cast<LPARAM>(&(pWnd->_hTreadGetFileList)));

	return 1;
}
//////////////////////////////////////////////////////////////////////////
TString GUIWnd::GetSessionID()
{
	TString session;

	Http http;
	if (!http.Init(true, AGENT_NAME))
		return session;

	int index = 0;

	if (_pConfig){
		index = _pConfig->getIndex();
	}

	ASSERT_SERVER_INDEX(index);

	static const char *szServer[3] = {
		"https://www.caresuite.nokia.com/caresuite/get_params.do?application_id=2",
		"https://www.dsut.online.nokia.com/oti/get_params.do?application_id=2",
		"https://www.dsut-qa.online.nokia.com/oti/get_params.do?application_id=2"
	};

	if (http.Get(szServer[index], true) != CURLE_OK)
		return session;

	DWORD dwSize = 0;

	char* hdrBuf = http.GetData(&dwSize);
	if (!hdrBuf){
		return session;
	}

	char *p = StrStrIA(hdrBuf, "JSESSIONID=");

	//fixed bug when server unavailable
	//Apr. 12, 2011
	if (!p){
		return session;
	}

	while (*p && *p != '=')
		p++;

	if (!*p){
		return session;
	}

	p++;	//skip '='

	if (!*p){
		return session;
	}

	session.clear();
	//由于JSESSIONID均字母数字，就不用转换了
	while (*p && *p != ';')
	{
		session.push_back(*p);
		p++;
	}

	return session;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::VariantDataToFiles(const char *data)
{
	if (!data)
		return ;

	TiXmlDocument *pXmlDoc =new TiXmlDocument;
	pXmlDoc->Parse(data);
	TString errInfo;

	if (pXmlDoc->Error())
		goto end;
	
	if (IsErrorInfo(pXmlDoc, errInfo))
	{
		UpdateStatus(TEXT("获取列表失败，请检查输入的CODE是否正确。。。"));

		TString str(TEXT("获取文件列表时发生错误，服务器返回信息：\r\n"));
		str += errInfo + TEXT("\r\n请检查CODE是否正确！");
		msgBox(str, TEXT("获取列表失败"), MB_ICONERROR);

		goto end;
	}

	TiXmlNode *nodeFiles = pXmlDoc->FirstChild("env:Envelope");
	if (!nodeFiles)
		goto end;

	nodeFiles = nodeFiles->FirstChild("env:Body");
	if (!nodeFiles)
		goto end;

	TiXmlNode *bodyNode = nodeFiles;

	nodeFiles = nodeFiles->FirstChild("m:GetVariantResponse");
	if (!nodeFiles)
		goto end;

	nodeFiles = nodeFiles->FirstChild("result");
	if (!nodeFiles)
		goto end;

	nodeFiles = nodeFiles->FirstChild("files");

	TiXmlNode *file = nodeFiles->FirstChild("file");

	UpdateStatus(TEXT("正在分析返回数据……"));

	for (; file; file = file->NextSibling())
	{
		FileInfo fi;
		fi.name = GetFileElement(file, "fileName");
		if (fi.name.empty())
			continue;

		TString sizeStr = GetFileElement(file, "fileSize");
		if (!sizeStr.empty()){
			fi.size = _ttol(sizeStr.c_str());
		}

		fi.url = GetFileElement(file, "downloadURL", bodyNode);
		if (fi.url.empty())
			continue;

		_vFiles.push_back(fi);
	}

end:
	if (pXmlDoc)
		delete pXmlDoc;
}
//////////////////////////////////////////////////////////////////////////
TString GUIWnd::GetFileElement(TiXmlNode *fileNode, const char *tagName, TiXmlNode *bodyNode/* = NULL*/)
{
	if (!fileNode || !tagName)
		return TEXT("");

	string valueStr;

	TiXmlElement *element = fileNode->FirstChildElement(tagName);
	if (element)
	{
		const char *value = element->GetText();
		if (value){
			valueStr = value;
		}
		else if (StrCmpIA(tagName, "downloadURL") == 0 && bodyNode)
		{	//<downloadURL href="#ID_11" /> 类型
			const char *id = element->Attribute("href");
			if (id){
				valueStr = GetFileUrlByID(bodyNode, ++id);	//去掉#
			}
		}
	}

	if (valueStr.empty())
		return TEXT("");

#ifdef UNICODE
	int len = atow(valueStr.c_str(), NULL);
	wchar_t *wvalue = new wchar_t[len];
	atow(valueStr.c_str(), wvalue, len);
	TString str(wvalue);
	delete [] wvalue;
	return str;
#else
	return valueStr.c_str();
#endif
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ShowFiles()
{
	TString tmpStr;
	tmpStr.format(TEXT("(%d)"), _vFiles.size());
	::SendMessage(HwndFromId(IDS_FILE_LIST), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(tmpStr.c_str()));
	InvalidStatic(IDS_FILE_LIST);

	_lvFile.DeleteAllItems();

	if (_vFiles.empty())
		return ;

	LVITEM lvi = {0};

	lvi.cchTextMax = 256;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = _lvFile.GetItemCount();
	
	for (size_t i=0; i<_vFiles.size(); i++)
	{
		lvi.pszText = (TCHAR*)_vFiles[i].name.c_str();
		_lvFile.InsertItem(&lvi);
		_lvFile.SetItemText(lvi.iItem, 1, (TCHAR*)MakeFileSizeFmt(_vFiles[i].size).c_str());

		_lvFile.SetCheck(lvi.iItem);

		lvi.iItem ++;
	}
}
//////////////////////////////////////////////////////////////////////////
bool GUIWnd::IsErrorInfo(TiXmlDocument *pDoc, TString &strErrInfo)
{
	if (!pDoc){
		return true;
	}

	TiXmlNode *node = pDoc->FirstChild("env:Envelope");
	if (!node){
		return true;
	}
	node = node->FirstChild("env:Body");
	if (!node){
		return true;
	}

	TiXmlNode *nodeFault = node->FirstChild("env:Fault");
	if (!nodeFault){
		return false;
	}

	node = nodeFault->FirstChild("detail");
	if (node){
		strErrInfo = GetChildElementText(node, "Description");
	}else{
		strErrInfo = GetChildElementText(nodeFault, "faultstring");
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
TString GUIWnd::GetChildElementText(TiXmlNode *node, const char *childElement)
{
	TString result;

	if (!node){
		return result;
	}

	TiXmlElement *element = node->FirstChildElement(childElement);
	if (element)
	{
		const char *value = element->GetText();
		if (value)
		{
#ifdef UNICODE
			int len = atow(value, NULL);
			wchar_t *wvalue = new wchar_t[len];
			atow(value, wvalue, len);
			result = wvalue;
			delete [] wvalue;
#else
			result = value;
#endif
		}
	}

	return result;
}
//////////////////////////////////////////////////////////////////////////
bool GUIWnd::SaveExportText(LPCTSTR lpDefFileName, const TString &strData)
{
	TString filePath = SelectFilePath(lpDefFileName);
	if (filePath.empty())
		return false;

#ifdef UNICODE
	int len = wtoa(strData, NULL);
	char *buffer = new char[len];
	wtoa(strData, buffer, len);

	bool fOk = SaveToFile(filePath, (LPVOID)buffer, len);

	delete [] buffer;
#else
	bool fOk = SaveToFile(filePath, (LPVOID)strData.c_str(), strData.length());
#endif

	return fOk;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::UpdateStatus(const TString &infoText, int nPart /* = 1 */)
{
	_status.setText(nPart, infoText);
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ShowServer(int index /* = 1 */)
{
	static const TCHAR *szServer[3] =
	{
		TEXT("\tNokia Care Suit"),
		TEXT("\tProduction External"),
		TEXT("\tQuality Assurance")
	};
	UpdateStatus(szServer[index], 3);
}
//////////////////////////////////////////////////////////////////////////
//文件列表排序
//////////////////////////////////////////////////////////////////////////
bool file_sort_by_name(const FileInfo &lhs, const FileInfo &rhs)
{
	if (gfsortUp)
		return lhs.name < rhs.name;
	else
		return lhs.name > rhs.name;
}
//////////////////////////////////////////////////////////////////////////
bool file_sort_by_size(const FileInfo &lhs, const FileInfo &rhs)
{
	if (gfsortUp)
		return lhs.size < rhs.size;
	else
		return lhs.size > rhs.size;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::SortListItem(int index)
{
	if (index >= 2)
		return ;

	HWND hLvHdr = _lvFile.GetHeader();
	if (!hLvHdr)	return ;

	HDITEM hdi = {0};
	hdi.mask = HDI_FORMAT;

	for (int i=0; i<2; i++)
	{
		if (i == index)
		{
			if (_sortIndex != index)	//要排序的列
			{
				hdi.fmt = HDF_SORTUP | HDF_STRING;
				gfsortUp = true;

				_sortIndex = index;
				_sortType = SORT_UP;
			}
			else		//已标记排序列
			{
				if (_sortType == SORT_UP)
				{
					hdi.fmt = HDF_SORTDOWN | HDF_STRING;
					gfsortUp = false;
					_sortType = SORT_DOWN;
				}
				else
				{
					hdi.fmt = HDF_SORTUP | HDF_STRING;
					gfsortUp = true;
					_sortType = SORT_UP;
				}
			}
		}
		else
			hdi.fmt	 = HDF_STRING;

		Header_SetItem(hLvHdr, i, &hdi);
	}

	switch(index)
	{
	case 0:
		sort(_vFiles.begin(), _vFiles.end(), file_sort_by_name);
		break;
	case 1:
		sort(_vFiles.begin(), _vFiles.end(), file_sort_by_size);
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
char* GUIWnd::GetFileDataFromZip(const TString& zipFile, const TString& name, DWORD& dwSize)
{
	HZIP hZip = OpenZip(zipFile, NULL);
	if (!hZip){
		return NULL;
	}

	ZIPENTRY ze = {0};
	int index = 0;

	if (FindZipItem(hZip, name, true, &index, &ze) == ZR_OK && index != -1)
	{
		dwSize = ze.unc_size + 1;
		char* buffer = new char[dwSize];
		RtlSecureZeroMemory(buffer, dwSize);
		if (UnzipItem(hZip, index, buffer, ze.unc_size) == ZR_OK)
		{
			CloseZip(hZip);
			return buffer;
		}
		delete [] buffer;
	}

	CloseZip(hZip);

	return NULL;
}
//////////////////////////////////////////////////////////////////////////
bool GUIWnd::SaveFileToZip(const TString& zipFile, const TString& name, LPVOID lpData, DWORD dwSize)
{
	HZIP hZip = NULL;
	map<TString, TString> mTmpFiles;
	TCHAR tmp[MAX_PATH] = {0};

	if (::PathFileExists(zipFile))
	{
		hZip = OpenZip(zipFile, NULL);

		if (hZip)
		{
			ZIPENTRY ze = {0};

			if (GetZipItem(hZip, -1, &ze) == ZR_OK)
			{

				if (GetTempPath(MAX_PATH, tmp) > 0)
					lstrcat(tmp, TEXT("nfetmp"));
				else
					lstrcpy(tmp, TEXT("temp"));

				size_t total = ze.index;
				TString path;

				for (size_t i=0; i<total; i++)
				{
					if (GetZipItem(hZip, i, &ze) == ZR_OK)
					{
						if (StrCmpI(ze.name, name) == 0)
							continue;

						path.format(TEXT("%s\\%s"), tmp, ze.name);
						if (UnzipItem(hZip, i, path) == ZR_OK)
							mTmpFiles[ze.name] = path;
					}
				}
			}
		}
	}

	if (hZip){
		CloseZip(hZip);
	}
	
	hZip = CreateZip(zipFile, 0);

	if (!hZip){
		return false;
	}
	
	if (ZR_OK != ZipAdd(hZip, name, lpData, dwSize))
	{
		CloseZip(hZip);
		return false;
	}

	for (map<TString, TString>::iterator it = mTmpFiles.begin();
		it != mTmpFiles.end(); ++it){
		ZipAdd(hZip, it->first, it->second);
	}

	CloseZip(hZip);

	if (tmp[0]){
		File::RmDir(tmp);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////
bool GUIWnd::FileExistsZip(const TString& zipFile, const TString& name)
{
	HZIP hZip = OpenZip(zipFile, NULL);
	if (!hZip){
		return false;
	}

	ZIPENTRY ze = {0};
	int index = 0;

	if (FindZipItem(hZip, name, true, &index, &ze) == ZR_OK)
	{
		if (index != -1)
		{
			CloseZip(hZip);
			return true;
		}
	}

	CloseZip(hZip);

	return false;
}
//////////////////////////////////////////////////////////////////////////
char* GUIWnd::GetSoap(Http& http, LPCTSTR lpPost, LPDWORD pdwSize)
{
	if (!http.Init(true, AGENT_NAME))
	{
		UpdateStatus(TEXT("LibCurl初始化出错了！"));
		return 0;
	}

	http.SetHeader("Content-Type: text/xml");

	int index = 0;
	if (_pConfig){
		index = _pConfig->getIndex();
	}

	ASSERT_SERVER_INDEX(index);

	UpdateStatus(TEXT("正在连接服务器获取数据……"));

	http.ShowProgress(WebProgress, this);
#ifdef UNICODE
	if (http.Post(_szServer[index], (LPVOID)wtoa(lpPost).c_str()) != CURLE_OK)
#else
	if (http.Post(_szServer[index], (LPVOID)lpPost) != CURLE_OK)
#endif
		return NULL;

	return http.GetData(pdwSize);
}
//////////////////////////////////////////////////////////////////////////
TString GUIWnd::TryGetSessionID()
{
	TString &session = _jsessionidStr;

	if (session.empty())
	{
		UpdateStatus(TEXT("尝试获取SessionID..."));
		session = GetSessionID();
	}

	if (session.empty())
	{
		msgBox(TEXT("无法获取有效身份验证ID，请稍后再尝试！\r\n"),
			TEXT("出错"), MB_ICONERROR);
	}

	return session;
}
//////////////////////////////////////////////////////////////////////////
TString GUIWnd::LoadSoapString(UINT uID)
{
	TCHAR* buffer = new TCHAR[750];
	LoadString(getHinst(), uID, buffer, 750);

	TString str = buffer;
	delete [] buffer;

	return str;
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::WebProgress(double total, double now, void* pGuiWnd)
{
	GUIWnd* pWnd = (GUIWnd*)pGuiWnd;

	TString str = MakeFileSizeFmt((DWORD)now);
	pWnd->UpdateStatus(str, 2);
}
//////////////////////////////////////////////////////////////////////////
void GUIWnd::ValidTaskMgr()
{
	//未创建过
	if (_taskMgr == NULL)
	{
		try
		{
			_taskMgr = new TaskMgrWnd;
			_taskMgr->init(getHinst(), getSelf());
		}
		catch(std::exception &e)
		{
			MessageBoxA(getSelf(), e.what(), "C++ Exception", MB_ICONERROR);
			return ;
		}
	}

	if (_newTaskDlg == NULL)
	{
		_newTaskDlg = new DlgNewTask;
		_newTaskDlg->init(getHinst(), getSelf());

		_newTaskDlg->create(IDD_NEW_TASK, _taskMgr);
	}
}

void GUIWnd::InitTaskBar()
{	
	if(SUCCEEDED(CoInitialize(NULL)))
	{
		HRESULT hr = CoCreateInstance(CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&_pTaskbar));
		if (SUCCEEDED(hr))
		{
			hr = _pTaskbar->HrInit();
			if (FAILED(hr))
			{
				_pTaskbar->Release();
				_pTaskbar = NULL;
			}
		}
	}
}

void GUIWnd::ShowIndeterminateProgress(bool fShow /* = true */)
{
	if (_fDisableTaskbar){
		return ;
	}

	if (_fDownloading){
		return ;	//正在下载时不起作用
	}

	if (!_pTaskbar){
		return ;
	}

	if (fShow){
		_pTaskbar->SetProgressState(_hWnd, TBPF_INDETERMINATE);
	}else{
		_pTaskbar->SetProgressState(_hWnd, TBPF_NOPROGRESS);
	}
}

void GUIWnd::InvalidStatic(UINT id)
{
	Rect rect;
	::GetWindowRect(HwndFromId(id), &rect);
	ScreenToClient(&rect);
	InvalidateRect(&rect, TRUE);
}

void GUIWnd::ExcludeChildRect(HDC hdc)
{
	Rect rc;

	vector<Rect> vRect;

	::GetWindowRect(HwndFromId(IDE_PRODUCT_FILTER), &rc);
	vRect.push_back(rc);

	_lbProduct.GetWindowRect(&rc);
	vRect.push_back(rc);

	_lbRelease.GetWindowRect(&rc);
	vRect.push_back(rc);

	::GetWindowRect(HwndFromId(IDE_VARIANT_FILTER), &rc);
	vRect.push_back(rc);

	_lbVariant.GetWindowRect(&rc);
	vRect.push_back(rc);

	_lvFile.GetWindowRect(&rc);
	vRect.push_back(rc);

	_status.GetWindowRect(&rc);
	vRect.push_back(rc);

	for (size_t i=0; i<vRect.size(); i++)
	{
		ScreenToClient(&vRect[i]);
		ExcludeClipRect(hdc, vRect[i].left, vRect[i].top, vRect[i].right, vRect[i].bottom);
	}
}

void GUIWnd::InitBackground()
{
	if (_pConfig == NULL)
		return ;

	if (_hbmpBkgnd)
		DeleteObject(_hbmpBkgnd);

	COLORREF cr1, cr2;
	_pConfig->getColor(cr1, cr2);
	_hbmpBkgnd = GradienBitmap(_hWnd, cr1, cr2);

	InvalidateRect(NULL, TRUE);
}