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

#include <shlobj.h>

#include "TaskMgrWnd.h"
#include "Tim/ClassEx.h"
#include "res/resource.h"
#include "nm_message.h"
#include "Tim/File.h"
#include "Tim\SException.h"
#include "common.h"
#include "LangHelper.h"

using namespace std;
using namespace Tim;

#define SPEED_TIMER_ID		5
#define SPEED_REFRESH		1500		//�ٶ�ˢ�¼��

#define IDM_PAUSE			40300
#define	IDM_CONTINUE		40301
#define IDM_DELETE			40302
#define IDM_COPY_URL		40303

TaskMgrWnd::~TaskMgrWnd()
{
	if (_taskView){
		_taskView->destroy();
		delete _taskView;
	}

	if (!_vDownloader.empty())
	{
		for (size_t i=0; i<_vDownloader.size(); i++)
		{
			MiniDownloader *pObj = _vDownloader[i];

			if (pObj)	//Ԥ��"����"
			{
				pObj->Stop();
				delete pObj;
			}
		}
	}
}

void TaskMgrWnd::init(HINSTANCE hinst, HWND hwndParent)
{
	Window::init(hinst, hwndParent);

	ClassEx cls(hinst, TEXT("tiTaskMgr"), IDI_TASK, 0, WndProc);

	if (!cls.Register()){
		throw std::runtime_error("TaskMgrWnd::RegisterClassEx failed");
	}

	_hWnd = createEx(WS_EX_ACCEPTFILES,
		TEXT("tiTaskMgr"), TEXT("��������"),
		WS_OVERLAPPEDWINDOW,
		0, 0, 540, 250,
		0, (LPVOID)this);

	if (!_hWnd){
		throw std::runtime_error("TaskMgrWnd::CreateWindowEx failed");
	}

	Rect rect;
	BOOL fMaxed = FALSE;
	//ȡ���ϴδ���λ��
	::SendMessage(hwndParent, NM_GETTASKRECT, (WPARAM)&fMaxed, (LPARAM)&rect);

	if (!rect.IsRectEmpty())
	{
		moveTo(rect.left, rect.top);
		resizeTo(rect.Width(), rect.Height());
	}
	else
	{
		centerWnd();
	}

	if (fMaxed){
		sendMsg(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}

	//showWindow();
	UpdateWindow();

	SetTimer(SPEED_TIMER_ID, 500);
}

//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK TaskMgrWnd::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_NCCREATE :
		{
			TaskMgrWnd *pWnd = (TaskMgrWnd *)(((LPCREATESTRUCT)lParam)->lpCreateParams);
			pWnd->_hWnd = hWnd;
			::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)pWnd);
			return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
		}

	default:
		return ((TaskMgrWnd *)::GetWindowLongPtr(hWnd, GWL_USERDATA))->runProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK TaskMgrWnd::runProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_TIMER:
		OnTimer(wParam);
		break;

	case NM_DOWNPROGRESS:
		OnDownloadStatus((DownloadStatus*)lParam);
		break;

	case NM_DOWNFINISH:
		OnFinish(wParam, (MiniDownloader*)lParam);
		break;

	case WM_NOTIFY:
		OnNotify(int(wParam), (NMHDR*)lParam);
		break;

	case WM_COMMAND:
		OnCommand(LOWORD(wParam), HWND(lParam), HIWORD(wParam));
		break;

	case WM_SIZE:
		OnSize(wParam, LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_SYSCOMMAND:
		if (wParam == SC_MINIMIZE || wParam == SC_CLOSE)
		{
			hideWindow();
			break;
		}
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);

	case WM_CREATE:
		OnCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));
		break;

	case NM_ABOUTCLOSE:
		OnAboutClose();
		break;

	default:
		return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void TaskMgrWnd::OnCommand(int id, HWND hwndCtl, UINT uNotifyCode)
{
	int index = _taskView->GetSelectionMark();
	if (index == -1)
		return ;

	switch(id)
	{
	case IDM_PAUSE:
		if (_vDownloader[index]->Pause()){
			SetTaskStatus(index, ts_pause);
		}
		break;

	case IDM_CONTINUE:
		if (_vDownloader[index]->Resume()){
			SetTaskStatus(index, ts_downloding);
		}
		break;

	case IDM_DELETE:
		if (msgBox("DelTask", TEXT("ȷ��Ҫɾ���������������ɾ�����޷��ָ���"),
			TEXT("ɾ��ȷ��"), MB_ICONQUESTION | MB_YESNO) == IDYES)
		{
			if (!DeleteTask(index)){
				msgBox("DelErr", TEXT("ɾ��ʧ�ܣ�"), TEXT("����ʧ��"), MB_ICONERROR);
			}
		}
		break;

	case IDM_COPY_URL:
		{
			list<TiFile>::iterator it = _tiFileList.begin();
			for (int i=0; it != _tiFileList.end(); it++, i++)
			{
				if (i == index)
				{
#ifdef UNICODE
					if (!CopyTextToClipbrd(atow(it->url).c_str()))
#else
					if (!CopyTextToClipbrd(it->url))
#endif
						msgBox("Clipbrd", TEXT("����URL�����а�ʱ������"), TEXT("�޷�����"), MB_ICONERROR);
					break;
				}
			}
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
void TaskMgrWnd::OnSize(int type, int nWidth, int nHeight)
{
	_taskView->resizeTo(nWidth, nHeight);
}
//////////////////////////////////////////////////////////////////////////
void TaskMgrWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	_taskView = new ProgressView;
	_taskView->init(lpCreateStruct->hInstance, getSelf());
	_taskView->create();

	LVCOLUMN lvColumn = {0};

	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;

	static TCHAR *hdrName[] = 
	{
		TEXT("״̬"),
		TEXT("�ļ���"),
		TEXT("��С"),
		TEXT("����"),
		TEXT("�ٶ�")
	};

	int aWidth[] = {50, 130, 70, 190, 80};

	for (size_t i=0; i<5; i++)
	{
		lvColumn.pszText = hdrName[i];
		_taskView->InsertColumn(i, &lvColumn);
		_taskView->SetColumnWidth(i, aWidth[i]);
	}

	_taskView->SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	_taskView->SetProgressColumn(3);	//�趨������Ϊ������

	InitListImage();
}

void TaskMgrWnd::OnDownloadStatus(DownloadStatus *pds)
{
	if (!pds){
		return ;
	}

	try
	{		
		for (size_t i=0; i<_vDownloader.size(); i++)
		{
			if (_vDownloader[i])
			{
				if (pds->pdm == _vDownloader[i])
					_taskView->SetProgressPos(i, pds->now);
			}
		}
	}
	catch(const SException &e)
	{
		char info[256];

		wsprintfA(info, "TaskMgrWnd::OnDownloadStatus: ����[%s]�쳣", e.what());
		MessageBoxA(_hWnd, info, "�����쳣", MB_ICONERROR);
		return ;
	}

	int progress = _taskView->GetProgress();

	PostMessage(getParent(), NM_TASKBAR, progress, 0);
}

void TaskMgrWnd::OnFinish(BOOL bSuccess, MiniDownloader *pmd)
{
	size_t i = 0;
	vector<MiniDownloader*>::iterator mit = _vDownloader.begin();
	vector<long>::iterator sit = _vDownloadedSize.begin();
	list<TiFile>::iterator tit = _tiFileList.begin();

	try
	{
		for(; mit != _vDownloader.end();
			mit++, sit++, tit++, i++)
		{
			if (pmd == *mit)
			{
				if (bSuccess)
				{
					_taskView->DelItem(i);
					delete pmd;
					_vDownloader.erase(mit);
					_vDownloadedSize.erase(sit);
					_tiFileList.erase(tit);
				}
				else
				{
					SetTaskStatus(i, ts_failed);
				}
				break;
			}
		}
	}
	catch(const SException &e)
	{
		char info[256];

		wsprintfA(info, "TaskMgrWnd::OnFinish: ����[%s]�쳣", e.what());
		MessageBoxA(_hWnd, info, "�����쳣", MB_ICONERROR);

		return ;
	}

	if (_vDownloader.empty()){
		PostMessage(getParent(), NM_DOWNFINISH, 0, 0);
	}
}

void TaskMgrWnd::OnTimer(UINT uid)
{
	if (uid == SPEED_TIMER_ID)
	{
		for (size_t i=0; i<_vDownloadedSize.size(); i++)
		{
			long size = _vDownloader[i]->GetDownloadedSize();
			TString fmt = MakeSpeedFmt(size - _vDownloadedSize[i]);
			_taskView->SetItemText(i, 4, (TCHAR*)fmt.c_str());
			_vDownloadedSize[i] = size;
		}
	}
}

void TaskMgrWnd::OnNotify(int id, NMHDR *pnmh)
{
	switch(pnmh->code)
	{
	case NM_DBLCLK:	//˫�����������б�
		{
			LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE)pnmh;

			//Fiexed: Apr.23, 2011
			if (_vDownloader.empty() ||
				lpnmitem->iItem >= (int) _vDownloader.size()
				)
				return ;

			TaskStatus status = GetTaskStatus(lpnmitem->iItem);

			if (status == ts_downloding)
			{
				if (_vDownloader[lpnmitem->iItem]->Pause()){
					SetTaskStatus(lpnmitem->iItem, ts_pause);
				}
			}
			else if (status == ts_pause)
			{
				if (_vDownloader[lpnmitem->iItem]->Resume()){
					SetTaskStatus(lpnmitem->iItem, ts_downloding);
				}
			}
		}
		break;

	case NM_RCLICK:
		ShowPopupMenu();
		break;
	}
}

void TaskMgrWnd::OnAboutClose()
{
	KillTimer(SPEED_TIMER_ID);

	if (!_tiFileList.empty())
	{
		list<TiFile>::iterator it = _tiFileList.begin();
		size_t i = 0;

		try
		{
			for ( ; it != _tiFileList.end(); it++, i++)
			{
				MiniDownloader *pObj = _vDownloader[i];
				pObj->Stop();
				it->offset = pObj->GetDownloadedSize();
				delete pObj;
			}
		}
		catch(const SException &e)
		{
			char info[256];

			wsprintfA(info, "TaskMgrWnd::OnAboutClose: ����[%s]�쳣", e.what());

			MessageBoxA(_hWnd, info, "�����쳣", MB_ICONERROR);
			::PostMessage(getParent(), NM_ABOUTCLOSE, FALSE, NULL);

			return ;
		}

		//��Ҫ, ��Ȼ�����������ҵ�
		vector<MiniDownloader*>().swap(_vDownloader);

		::PostMessage(getParent(), NM_ABOUTCLOSE, TRUE, reinterpret_cast<LPARAM>(&_tiFileList));
	}
	else
	{
		::PostMessage(getParent(), NM_ABOUTCLOSE, FALSE, NULL);
	}
}

void TaskMgrWnd::newTask(const FileInfo &fileInfo)
{
	if (fileInfo.url.empty())
	{
		msgBox("NewTask_Url", TEXT("����URL����Ϊ�գ������������ʧ��"), TEXT("��������"), MB_ICONINFORMATION);
		return ;
	}

	TString fullPath = _baseFolder;

	if (!_baseFolder.empty())
	{
		if (_baseFolder.at(_baseFolder.length() - 1) != TEXT('\\'))
			_baseFolder.push_back(TEXT('\\'));
	}

	fullPath += fileInfo.name;

	MiniDownloader *pmd = new MiniDownloader;

	::SendMessage(getParent(), NM_SETPROXY, 0, (LPARAM)pmd);

	if (!pmd->Init(false))
	{
		msgBox("NewTask_init", TEXT("��ʼ��libcurlʧ�ܣ��޷��������أ�"), TEXT("������"), MB_ICONERROR);
		delete pmd;
		return ;
	}

	TiFile ti;

#ifdef UNICODE
	ti.setfile(wtoa(fullPath));
	ti.seturl(wtoa(fileInfo.url));
#else
	ti.setfile(fullPath);
	ti.seturl(fileInfo.url);
#endif
	ti.size = fileInfo.size;

	int item = _taskView->NewItem(fileInfo);
	SetTaskStatus(item, ts_downloding);

	if (!pmd->Download(getSelf(), ti))
	{
		_taskView->DelItem(item);
		msgBox("NewTask_Err", TEXT("�޷�����������������ʧ�ܣ�"), TEXT("����ʧ��"), MB_ICONERROR);
		
		delete pmd;

		return ;
	}

	_vDownloader.push_back(pmd);
	_vDownloadedSize.push_back(0);
	_tiFileList.push_back(ti);
}

/******************************************************************************************/
//�½�δ��������
/******************************************************************************************/
void TaskMgrWnd::newTask(const TiFile &tiFile)
{
	MiniDownloader *pmd = new MiniDownloader;

	if (!pmd->Init(false))
	{
		msgBox("NewTask_init", TEXT("��ʼ��libcurlʧ�ܣ��޷��������أ�"), TEXT("������"), MB_ICONERROR);
		delete pmd;
		return ;
	}

	::SendMessage(getParent(), NM_SETPROXY, 0, (LPARAM)pmd);

	FileInfo fileInfo;

	std::wstring title = File::GetFileName(atow(tiFile.file));
	if (title.empty()){
		title = atow(tiFile.file);
	}
#ifdef UNICODE
	fileInfo.name = title.c_str();
	fileInfo.url = atow(tiFile.url).c_str();
#else
	fileInfo.name = wtoa(title).c_str();
	fileInfo.url = tiFile.url;
#endif

	fileInfo.size = tiFile.size;

	int item = _taskView->NewItem(fileInfo);

	//Fixed
	//when the wanted file does not exists
	//then should download from the beginning
	//May. 1, 2011

	TiFile tmp = tiFile;
	if (!File::Exists(tiFile.file))
		tmp.offset = 0;

	//����ԭ���ϴ����ؽ��ȣ���ֹû��ʼʱ����0״̬
	_taskView->SetProgressPos(item, tmp.offset);

	SetTaskStatus(item, ts_downloding);

	if (!pmd->Download(getSelf(), tmp))
	{
		_taskView->DelItem(item);
		msgBox("NewTask_Err", TEXT("�޷�����������������ʧ�ܣ�"), TEXT("����ʧ��"), MB_ICONERROR);

		delete pmd;

		return ;
	}

	_vDownloader.push_back(pmd);
	_vDownloadedSize.push_back(0);
	_tiFileList.push_back(tiFile);
}

bool TaskMgrWnd::InitListImage()
{
	HIMAGELIST himl = NULL;

	himl = ImageList_Create(16, 16, ILC_COLOR32, 4, 0);
	if (himl == NULL){
		return false;
	}

	for (int id = IDI_DOWNLOADING; id <= IDI_FAILED; id++ )
	{
		HICON hIcon = LoadIcon(getHinst(), MAKEINTRESOURCE(id));
		ImageList_AddIcon(himl, hIcon);
	}

	if (ImageList_GetImageCount(himl) < 4)
		return false;

	_taskView->SetImageList(himl, LVSIL_SMALL);

	return true;
}

void TaskMgrWnd::SetTaskStatus(int i, TaskStatus status)
{
	LVITEM lvi = {0};

	lvi.iItem = i;
	lvi.mask = LVIF_IMAGE;
	lvi.iImage = status;

	_taskView->SetItem(&lvi);

	if (status == ts_pause || status == ts_finish){
		_taskView->SetProgressState(i, PBST_PAUSED);
	}else if (status == ts_downloding){
		_taskView->SetProgressState(i, PBST_NORMAL);
	}else if (status == ts_failed){
		_taskView->SetProgressState(i, PBST_ERROR);
	}
}

TString TaskMgrWnd::MakeSpeedFmt(long lSize)
{
	TString fmt;

	float size = float(lSize * 1000.0 / SPEED_REFRESH);

	if (size < 1024)	//B
	{
		fmt.format(TEXT("%.f B/s"), size);
	}
	else if(size >= 1024 && size < 1024 * 1024)	//KB
	{
		size /= 1024;
		fmt.format(TEXT("%.2f KB/s"), size);
	}
	else if(size >= 1024 * 1024 && size < 1024 * 1024 * 1024)
	{
		size /= 1024 * 1024;
		fmt.format(TEXT("%.2f MB/s"), size);
	}
	//û��ôţ��G�ɣ�

	return fmt;
}

bool TaskMgrWnd::hasTask() const
{
	int count = _taskView->GetItemCount();
	if ( count == 0){
		return false;
	}

	LVITEM lvi = {0};
	lvi.mask = LVIF_IMAGE;

	TaskStatus status = (TaskStatus)lvi.iImage;

	for (int i=0; i<count; i++)
	{
		lvi.iItem = i;

		if (_taskView->GetItem(&lvi))
		{
			//ֻ�����������ء���������
			if (lvi.iImage == ts_downloding)
				return true;
		}
	}

	return false;
}

void TaskMgrWnd::ShowPopupMenu()
{
	if (_taskView->GetItemCount() == 0)
		return ;

	HMENU hMenu = CreatePopupMenu();

	int i = _taskView->GetSelectionMark();

	MENUITEMINFO mii = {0};
	mii.cbSize = sizeof(MENUITEMINFO);
	mii.fMask = MIIM_BITMAP;
	HBITMAP hBitmap = NULL;

	if (i != -1)
	{
		TaskStatus status = GetTaskStatus(i);
		if (status == ts_downloding)
		{
			AppendMenu(hMenu, MF_STRING | MF_BYCOMMAND, IDM_PAUSE, TEXT("��ͣ����"));

			hBitmap = mii.hbmpItem = LoadBitmap(getHinst(), MAKEINTRESOURCE(IDB_PAUSE));
			SetMenuItemInfo(hMenu, IDM_PAUSE, MF_BYCOMMAND, &mii);

		}
		else if(status == ts_pause || status == ts_failed)
		{
			AppendMenu(hMenu, MF_STRING | MF_BYCOMMAND, IDM_CONTINUE, TEXT("��������"));

			hBitmap = mii.hbmpItem = LoadBitmap(getHinst(), MAKEINTRESOURCE(IDB_CONTINUE));;
			SetMenuItemInfo(hMenu, IDM_CONTINUE, MF_BYCOMMAND, &mii);
		}
	}

	AppendMenu(hMenu, MF_STRING | MF_BYCOMMAND, IDM_DELETE, TEXT("ɾ������"));

	mii.hbmpItem =  LoadBitmap(getHinst(), MAKEINTRESOURCE(IDB_DELETE));;
	SetMenuItemInfo(hMenu, IDM_DELETE, MF_BYCOMMAND, &mii);

	AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
	AppendMenu(hMenu, MF_STRING | MF_BYCOMMAND, IDM_COPY_URL, TEXT("������������"));

	SendMessage(getParent(), NM_SETPOPMENULANG, (WPARAM)hMenu, (LPARAM)"ProgressView");

	POINT pt;
	GetCursorPos(&pt);
	TrackPopupMenuEx(hMenu, TPM_BOTTOMALIGN, pt.x, pt.y, _hWnd, 0);

	DestroyMenu(hMenu);
	DeleteObject(mii.hbmpItem);
	DeleteObject(hBitmap);
}

TaskMgrWnd::TaskStatus TaskMgrWnd::GetTaskStatus(int i)
{
	LVITEM lvi = {0};
	lvi.mask = LVIF_IMAGE;
	lvi.iItem = i;

	if (!_taskView->GetItem(&lvi))
		return ts_downloding;

	return (TaskStatus)lvi.iImage;
}

bool TaskMgrWnd::DeleteTask(int i)
{
	if (i<0 || i>(int)_vDownloader.size())
		return false;

	vector<MiniDownloader*>::iterator mit = _vDownloader.begin();
	vector<long>::iterator sit = _vDownloadedSize.begin();
	list<TiFile>::iterator tit = _tiFileList.begin();

	try
	{	
		for(int j=0; mit != _vDownloader.end();
			mit++, sit++, tit++, j++)
		{
			if (j == i)
			{
				_taskView->DelItem(i);
				(*mit)->Stop();
				delete *mit;
				_vDownloader.erase(mit);
				_vDownloadedSize.erase(sit);

				File::Delete(tit->file);
				_tiFileList.erase(tit);

				if (_vDownloader.empty()){
					PostMessage(getParent(), NM_DOWNFINISH, 0, 0);
				}

				return true;
			}
		}
	}
	catch(const SException &e)
	{
		char info[256];

		wsprintfA(info, "TaskMgrWnd::DeleteTask: ����[%s]�쳣", e.what());
		MessageBoxA(_hWnd, info, "�����쳣", MB_ICONERROR);
	}

	return false;
}

int TaskMgrWnd::msgBox(LPCSTR type, const TString &text, const TString &caption /*= TEXT("MessageBox")*/, UINT uType/* = MB_OK*/)
{
	LangHelper lang;

	TString strCaption = caption;
	TString strInfo = text;
	TCHAR szLang[MAX_PATH] = {0};

	SendMessage(getParent(), NM_GETLANGPATH, 0, reinterpret_cast<LPARAM>(szLang));

	if (szLang[0])
	{
		if (lang.Load(szLang))
		{
			lang.GetMsgBox(type, strInfo, strCaption);
		}
	}

	return Window::msgBox(strInfo, strCaption, uType);
}