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
#include "Config.h"

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

	return _cfgDoc->SaveFile();
}

void Config::makeDefault()
{
	const char *cfg =
		"<NaviFirmEx version=\"1.5\" lang = \"\">"
			"<Window>"
				"<MainGUI left=\"0\" top=\"0\" right=\"600\" bottom=\"480\" maxed=\"0\">"
					"<Color begin = \"#BDB8ED\" end =\"#C0DACE\"/>"
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

		itemElement = itemElement->FirstChildElement("Color");

		if (itemElement)
		{
			const char *begin = itemElement->Attribute("begin");
			const char *end = itemElement->Attribute("end");

			_cr1 = makeColor(begin);
			_cr2 = makeColor(end);
		}

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