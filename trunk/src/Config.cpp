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

#include <tchar.h>
#include <algorithm>

#include "Config.h"
#include "Tim/TString.h"

using namespace Tim;

// key for rc4
#define RC4_KEY "NaviFirmEx_KEY"

// rc4
static char* rc4(const char *pszText, char *pszKey);
static char* byte2hex(const unsigned char *pszText, int len);
static unsigned char* hex2byte(const char *pszText);
static inline char int2hex(int num);
static inline int hex2int(char hex);

Config::Config(const char *filePath)
{
	if (!filePath)
	{
		MessageBox(0, TEXT("Need to set config file path first!"), TEXT("Error"), 0);
		return ;
	}

	lstrcpyA(_filePath, filePath);
	RtlSecureZeroMemory(_lastDir, MAX_PATH);

	_nIndex = 1;
	_cfgDoc = NULL;

	_tgMaxed = 0;
	_fGUIMaxed = 0;

	_cr1 = RGB(218, 220, 248);
	_cr2 = RGB(192, 218, 206);

	_fDownloadPrompt = TRUE;
	_fDownAction = TRUE;

	_fExitPrompt = TRUE;
	_fExitAction = TRUE;
	_fShowTaskMgr = TRUE;

	_crFont = RGB(0, 0, 0);
	RtlSecureZeroMemory(&_lFont, sizeof(LOGFONT));

	_tcscpy_s(_lFont.lfFaceName, 32, TEXT("ËÎÌו"));
	_lFont.lfHeight = 12;
}

bool Config::load()
{
	if (_cfgDoc){
		delete _cfgDoc;
	}

	_cfgDoc = new TiXmlDocument(_filePath);

	if (!_cfgDoc->LoadFile())
	{
		//MessageBoxA(0, doc.ErrorDesc(), doc.Value(), 0);
		return false;
	}

	TiXmlNode *node = _cfgDoc->FirstChild("NaviFirmEx");

	if (!node){
		return false;
	}

	const char *lang = node->ToElement()->Attribute("lang");
	if (lang){
		_langFile = lang;
	}

	TiXmlNode *nodeWindow = node->FirstChild("Window");
	if (nodeWindow){
		loadWindow(nodeWindow);
	}

	TiXmlNode *nodeDownload = node->FirstChild("Download");
	if (nodeDownload){
		loadDownload(nodeDownload);
	}

	TiXmlNode *nodeProxy = node->FirstChild("Proxy");
	if (nodeProxy){
		loadProxy(nodeProxy);
	}

	//fixed....
	//Mar. 11, 2011
	if (_nIndex < 0 || _nIndex >= 3)
		_nIndex = 1;

	return true;
}

bool Config::save()
{
	makeDefault();

	TiXmlNode *mainNode = _cfgDoc->FirstChild("NaviFirmEx");

	if (!mainNode)
		return false;

	mainNode->ToElement()->SetAttribute("lang", _langFile);

	TiXmlNode *nodeWindow = mainNode->FirstChild("Window");

	if (nodeWindow){
		writeWindow(nodeWindow);
	}

	TiXmlNode *nodeDownload = mainNode->FirstChild("Download");

	if (nodeDownload){
		writeDownload(nodeDownload);
	}

	TiXmlNode *nodeProxy = mainNode->FirstChild("Proxy");
	if (nodeProxy){
		writeProxy(nodeProxy);
	}

	return _cfgDoc->SaveFile();
}

void Config::makeDefault()
{
	const char *cfg =
		"<NaviFirmEx version=\"1.6\" lang = \"\">"
			"<Window>"
				"<MainGUI left=\"0\" top=\"0\" right=\"600\" bottom=\"480\" maxed=\"0\">"
					"<Color begin = \"#BDB8ED\" end =\"#C0DACE\"/>"
					"<Font>"
						"<Data value = \"12|0|0|0|0|0|0|0|0|0|0|0|0\"/>"
						"<Name value = \"ËÎÌו\"/>"
						"<Color value = \"#000000\"/>"
					"</Font>"
				"</MainGUI>"
				"<TaskMgr left=\"0\" top=\"0\" right=\"540\" bottom=\"250\" maxed=\"0\"/>"
			"</Window>"
			"<Download>"
				"<Server>1</Server>"
				"<Folder>0</Folder>"
				"<Prompt>"
					"<Exit action=\"1\" ask=\"1\"/>"
					"<Continue action=\"1\" ask=\"1\"/>"
				"</Prompt>"
				"<TaskMgr show=\"1\"/>"
			"</Download>"
			"<Proxy type=\"0\">"
				"<addr value = \"\"/>"
				"<port value = \"\"/>"
				"<usr value = \"\"/>"
				"<pwd value = \"\"/>"
			"</Proxy>"
		"</NaviFirmEx>"
		;

	if (_cfgDoc){
		delete _cfgDoc;
	}

	_cfgDoc = new TiXmlDocument(_filePath);
	_cfgDoc->Parse(cfg);
}

COLORREF Config::makeColor(const char *color)
{
	if (!color){
		return COLOR_WINDOW;
	}

	if (strlen(color) != 7){
		return COLOR_WINDOW;
	}

	color++;	//skip '#'

	int r = 0,
		 g = 0,
		 b = 0;

	r = HexToDec(color[0]) * 16 + HexToDec(color[1]);
	g = HexToDec(color[2]) * 16 + HexToDec(color[3]);
	b = HexToDec(color[4]) * 16 + HexToDec(color[5]);

	return RGB(r, g, b);
}

char *Config::makeColor(COLORREF cr)
{
	char *value = new char[8];

	wsprintfA(value, "#%02X%02X%02X", GetRValue(cr), GetGValue(cr), GetBValue(cr));

	return value;
}

int Config::HexToDec(char hex)
{
	if (hex >= 'a'){
		return hex - 'a' + 10;

	}else if(hex >= 'A'){
		return hex - 'A' + 10;

	}else{
		return hex - '0';
	}
}

void Config::loadWindow(TiXmlNode *node)
{
	TiXmlElement* itemElement = node->FirstChildElement("MainGUI");
	if (itemElement)
	{
		int x = 0, y = 0;

		itemElement->Attribute("left", (int*)&_rcGUI.left);
		itemElement->Attribute("top", (int*)&_rcGUI.top);
		itemElement->Attribute("right", (int*)&_rcGUI.right);
		itemElement->Attribute("bottom", (int*)&_rcGUI.bottom);
		itemElement->Attribute("maxed", &_fGUIMaxed);

		TiXmlElement *item = itemElement->FirstChildElement("Color");

		if (item)
		{
			const char *begin = item->Attribute("begin");
			const char *end = item->Attribute("end");

			_cr1 = makeColor(begin);
			_cr2 = makeColor(end);
		}

		loadFont(itemElement->FirstChild("Font"));

		itemElement = node->FirstChildElement("TaskMgr");
		if (itemElement)
		{
			itemElement->Attribute("left", (int*)&_rcTask.left);
			itemElement->Attribute("top", (int*)&_rcTask.top);
			itemElement->Attribute("right", (int*)&_rcTask.right);
			itemElement->Attribute("bottom", (int*)&_rcTask.bottom);
			itemElement->Attribute("maxed", &_tgMaxed);
		}
	}
}

void Config::loadDownload(TiXmlNode *node)
{
	TiXmlElement *itemElement = node->FirstChildElement("Folder");
	if (itemElement)
	{
		const char *path = itemElement->GetText();
		if (path){
			lstrcpyA(_lastDir, path);
		}
	}

	itemElement = node->FirstChildElement("Server");
	if (itemElement)
	{
		const char *value = itemElement->GetText();
		if (value){
			_nIndex = atoi(value);
		}
	}

	TiXmlNode *nodePrompt = node->FirstChild("Prompt");

	if (nodePrompt)
	{
		itemElement = nodePrompt->FirstChildElement("Exit");
		if (itemElement)
		{
			itemElement->Attribute("action", &_fExitAction);
			itemElement->Attribute("ask", &_fExitPrompt);
		}

		itemElement = nodePrompt->FirstChildElement("Continue");
		if (itemElement)
		{
			itemElement->Attribute("action", &_fDownAction);
			itemElement->Attribute("ask", &_fDownloadPrompt);
		}
	}

	itemElement = node->FirstChildElement("TaskMgr");
	if (itemElement){
		itemElement->Attribute("show", &_fShowTaskMgr);
	}
}

void Config::writeWindow(TiXmlNode *node)
{
	TiXmlElement* itemElement = node->FirstChildElement("MainGUI");
	if (itemElement)
	{
		itemElement->SetAttribute("left", _rcGUI.left);
		itemElement->SetAttribute("top", _rcGUI.top);
		itemElement->SetAttribute("right", _rcGUI.right);
		itemElement->SetAttribute("bottom", _rcGUI.bottom);
		itemElement->SetAttribute("maxed", _fGUIMaxed);

		TiXmlElement *color = itemElement->FirstChildElement("Color");
		if (color)
		{
			char *cr1 = makeColor(_cr1);
			char *cr2 = makeColor(_cr2);

			color->SetAttribute("begin", cr1);
			color->SetAttribute("end", cr2);

			delete [] cr1;
			delete [] cr2;
		}

		writeFont(itemElement);
	}

	itemElement = node->FirstChildElement("TaskMgr");
	if (itemElement)
	{
		itemElement->SetAttribute("left", _rcTask.left);
		itemElement->SetAttribute("top", _rcTask.top);
		itemElement->SetAttribute("right", _rcTask.right);
		itemElement->SetAttribute("bottom", _rcTask.bottom);
		itemElement->SetAttribute("maxed", _tgMaxed);
	}
}

void Config::writeDownload(TiXmlNode *node)
{
	TiXmlElement* itemElement = node->FirstChildElement("Server");

	if (itemElement)
	{
		char value[3] = {0};
		_itoa(_nIndex, value, 10);
		TiXmlText text(value);
		TiXmlNode *child = itemElement->FirstChild();
		if (child){
			itemElement->ReplaceChild(child, text);
		}
	}

	itemElement = node->FirstChildElement("Folder");
	if (itemElement)
	{
		TiXmlText text(_lastDir);
		TiXmlNode *child = itemElement->FirstChild();
		if (child){
			itemElement->ReplaceChild(child, text);
		}
	}

	TiXmlNode *nodePrompt = node->FirstChild("Prompt");
	if (nodePrompt)
	{
		itemElement = nodePrompt->FirstChildElement("Exit");
		itemElement->SetAttribute("action", _fExitAction);
		itemElement->SetAttribute("ask", _fExitPrompt);

		itemElement = nodePrompt->FirstChildElement("Continue");
		itemElement->SetAttribute("action", _fDownAction);
		itemElement->SetAttribute("ask", _fDownloadPrompt);
	}

	itemElement = node->FirstChildElement("TaskMgr");
	if (itemElement){
		itemElement->SetAttribute("show", _fShowTaskMgr);
	}
}

void Config::writeFont(TiXmlNode *node)
{
	TiXmlNode *font = node->FirstChild("Font");
	if (font)
	{
		TiXmlElement *item = font->FirstChildElement("Data");
		if (item)
		{
			char data[128] = {0};
			wsprintfA(data, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d", _lFont.lfHeight,
				_lFont.lfWidth, _lFont.lfEscapement, _lFont.lfOrientation, _lFont.lfWeight,
				_lFont.lfItalic, _lFont.lfUnderline, _lFont.lfStrikeOut, _lFont.lfCharSet,
				_lFont.lfOutPrecision, _lFont.lfClipPrecision, _lFont.lfQuality, _lFont.lfPitchAndFamily
				);

			item->SetAttribute("value", data);
		}

		item = font->FirstChildElement("Name");
		if (item)
		{
#ifdef UNICODE
			item->SetAttribute("value", wtoa(_lFont.lfFaceName));
#else
			item->SetAttribute("value", _lFont.lfFaceName);
#endif
		}

		item = font->FirstChildElement("Color");
		if (item)
		{
			item->SetAttribute("value", makeColor(_crFont));
		}
	}
}

void Config::loadFont(TiXmlNode *node)
{
	if (!node)
		return ;

	TiXmlElement *item  = node->FirstChildElement("Data");
	if (item)
	{
		const char *data = item->Attribute("value");
		if (data)
		{
			sscanf_s(data, "%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d|%d",  &_lFont.lfHeight,
				&_lFont.lfWidth, &_lFont.lfEscapement, &_lFont.lfOrientation, &_lFont.lfWeight,
				&_lFont.lfItalic, &_lFont.lfUnderline, &_lFont.lfStrikeOut, &_lFont.lfCharSet,
				&_lFont.lfOutPrecision, &_lFont.lfClipPrecision, &_lFont.lfQuality,
				&_lFont.lfPitchAndFamily)
				;
		}
	}

	item = node->FirstChildElement("Name");
	if (item)
	{
		const char *value = item->Attribute("value");
		if (value)
		{
			TString str(value);
			str.trim();
			_tcscpy_s(_lFont.lfFaceName, 32, str);
		}
	}

	item = node->FirstChildElement("Color");
	if (item)
	{
		const char *value = item->Attribute("value");
		if (value)
			_crFont = makeColor(value);
	}
}

void Config::loadProxy(TiXmlNode *node)
{
	if (NULL == node)
		return ;

	const char *value = node->ToElement()->Attribute("type");
	if (value)
		_proxy.type = atoi(value);

	TiXmlElement *item = node->FirstChildElement("addr");
	if (item)
	{
		value = item->Attribute("value");
		if (value)
			_proxy.server = value;
	}

	item = node->FirstChildElement("port");
	if (item)
	{
		value = item->Attribute("value");
		if (value)
			_proxy.port = atoi(value);
	}

	item = node->FirstChildElement("usr");
	if (item)
	{
		value = item->Attribute("value");
		if (value)
			_proxy.usr = value;
	}

	item = node->FirstChildElement("pwd");
	if (item)
	{
		value = item->Attribute("value");
		if (value)
		{
			unsigned char *cipher = hex2byte(value);
			if (NULL != cipher)
			{
				char *pwd = rc4((char*)cipher, RC4_KEY);
				_proxy.pwd = pwd;

				delete [] cipher;
				delete [] pwd;
			}
		}
	}
}

void Config::writeProxy(TiXmlNode *node)
{
	if (NULL == node)
		return ;

	node->ToElement()->SetAttribute("type", _proxy.type);

	TiXmlElement *item = node->FirstChildElement("port");
	if (item)
		item->SetAttribute("value", _proxy.port);

#ifdef UNICODE
	item = node->FirstChildElement("addr");
	if (item)
		item->SetAttribute("value", wtoa(_proxy.server));

	item = node->FirstChildElement("usr");
	if (item)
		item->SetAttribute("value", wtoa(_proxy.usr));

	std::string pwd = wtoa(_proxy.pwd);
	char *cipher = rc4(pwd.c_str(), RC4_KEY);
	char *hex_pwd = byte2hex((unsigned char*)cipher, strlen(cipher));

	item = node->FirstChildElement("pwd");
	if (item)
		item->SetAttribute("value", hex_pwd);
#else
	item = node->FirstChildElement("addr");
	if (item)
		item->SetAttribute("value", _proxy.server);

	item = node->FirstChildElement("usr");
	if (item)
		item->SetAttribute("value", _proxy.usr);

	char *cipher = rc4(_proxy.pwd, RC4_KEY);
	char *hex_pwd = byte2hex(cipher, strlen(cipher));

	item = node->FirstChildElement("pwd");
	if (item)
		item->SetAttribute("value", hex_pwd);
#endif

	delete [] cipher;
	delete [] hex_pwd;
}

static char* rc4(const char *pszText, char *pszKey)
{
	int sbox[256];
	int key[256];

	int iLen = strlen(pszKey);

	for (int i=0; i<256; i++)
	{
		key[i] = pszKey[i % iLen];
		sbox[i] = i;
	}

	for (int i=0, j=0; i<256; i++)
	{
		j = (j + sbox[i] + key[i]) % 256;
		std::swap(sbox[i], sbox[j]);
	}

	iLen = strlen(pszText);

	char *cipher = new char[iLen + 1];
	memset(cipher, 0, iLen + 1);
	int j = 0, k = 0;

	for (int i=0; i<iLen; i++)
	{
		j = (j + 1) % 256;
		k = (k + sbox[j]) % 256;
		std::swap(sbox[j], sbox[k]);
		int m = sbox[(sbox[j] + sbox[k]) % 256];
		cipher[i] = pszText[i] ^ m;
	}

	return cipher;
}

static char* byte2hex(const unsigned char *pszText, int len)
{
	if(NULL == pszText)
		return NULL;

	char *hex = new char[len * 2 + 1];
	memset(hex, 0, len * 2 + 1);

	for (int i=0; i<len; i++)
	{
		int digit = int(pszText[i]) / 16;
		hex[i * 2] = int2hex(digit);
		digit = int(pszText[i]) % 16;

		hex[i * 2 + 1] = int2hex(digit);
	}

	return hex;
}

static unsigned char* hex2byte(const char *pszText)
{
	if(NULL == pszText)
		return NULL;

	int iLen = strlen(pszText);

	if (iLen <= 0 || 0 != iLen % 2)
		return NULL;

	iLen /= 2;
	unsigned char *bytes = new unsigned char[iLen + 1];
	memset(bytes, 0, iLen + 1);

	for (int i=0; i<iLen; i++)
	{
		int p1 = hex2int(pszText[i * 2]);
		int p2 = hex2int(pszText[i * 2 + 1]);

		bytes[i] = char(p1*16 + p2);
	}

	return bytes;
}

static inline char int2hex(int num)
{
	if (num > 9)
		return 'A' + num - 10;

	return num + '0';
}

static inline int hex2int(char hex)
{
	if (hex >= 'A')
		return hex - 'A' + 10;

	return hex - '0';
}