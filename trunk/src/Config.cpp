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
	if (!filePath){
		throw TEXT("Need to set config file path first!");
	}

	lstrcpyA(_filePath, filePath);
	RtlSecureZeroMemory(_lastDir, MAX_PATH);

	_pos.x = _pos.y = 100;
	_nIndex = 1;
	_cfgDoc = NULL;
	_tgMaxed = 0;
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

	if (!node)
		return false;

	TiXmlElement * itemWindow = node->FirstChildElement("Window");
	if (!itemWindow)
		return false;

	TiXmlElement* itemElement = itemWindow->FirstChildElement("Position");
	if (!itemElement)
		return false;

	int x = 0, y = 0;

	itemElement->Attribute("x", &x);
	itemElement->Attribute("y", &y);

	_pos.x = x < 0 ? 0 : x;
	_pos.y = y < 0 ? 0 : y;

	itemElement = itemWindow->FirstChildElement("TaskMgr");
	if (itemElement)
	{
		itemElement->Attribute("left", (int*)&_rcTask.left);
		itemElement->Attribute("top", (int*)&_rcTask.top);
		itemElement->Attribute("right", (int*)&_rcTask.right);
		itemElement->Attribute("bottom", (int*)&_rcTask.bottom);
		itemElement->Attribute("maxed", &_tgMaxed);
	}

	itemElement = itemWindow->FirstChildElement("Folder");
	if (itemElement)
	{
		const char *path = itemElement->Attribute("path");
		if (path){
			lstrcpyA(_lastDir, path);
		}
	}

	itemElement = node->FirstChildElement("Server");
	if (!itemElement)
		return false;

	itemElement->Attribute("index", &_nIndex);

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

	TiXmlElement *itemWindow = mainNode->FirstChildElement("Window");

	if (itemWindow)
	{
		TiXmlElement* itemElement = itemWindow->FirstChildElement("Position");
		if (itemElement)
		{
			itemElement->SetAttribute("x", _pos.x);
			itemElement->SetAttribute("y", _pos.y);
		}

		itemElement = itemWindow->FirstChildElement("TaskMgr");
		if (itemElement)
		{
			itemElement->SetAttribute("left", _rcTask.left);
			itemElement->SetAttribute("top", _rcTask.top);
			itemElement->SetAttribute("right", _rcTask.right);
			itemElement->SetAttribute("bottom", _rcTask.bottom);
			itemElement->SetAttribute("maxed", _tgMaxed);
		}

		itemElement = itemWindow->FirstChildElement("Folder");
		if (itemElement){
			itemElement->SetAttribute("path", _lastDir);
		}
	}
	else
	{
		TiXmlElement wnd("Window");
		TiXmlElement pos("Position");

		pos.SetAttribute("x", _pos.x);
		pos.SetAttribute("y", _pos.y);

		wnd.InsertEndChild(pos);

		_cfgDoc->FirstChildElement("NaviFirmEx")->InsertEndChild(wnd);
	}

	TiXmlElement* itemElement = mainNode->FirstChildElement("Server");

	if (itemElement){
		itemElement->SetAttribute("index", _nIndex);
	}
	else
	{
		TiXmlElement item("Server");

		item.SetAttribute("index", _nIndex);

		_cfgDoc->FirstChildElement("NaviFirmEx")->InsertEndChild(item);
	}

	return _cfgDoc->SaveFile();
}

void Config::makeDefault()
{
	const char *cfg =
		"<NaviFirmEx version=\"1.3\">"
		"<Window>"
		"<Position x = \"100\" y = \"100\"/>"
		"<TaskMgr left=\"0\" top=\"0\" right=\"540\" bottom=\"250\" maxed=\"0\"/>"
		"<Folder path = \"\"/>"
		"</Window>"
		"<Server index = \"0\"/>"
		"</NaviFirmEx>"
		;

	if (_cfgDoc){
		delete _cfgDoc;
	}

	_cfgDoc = new TiXmlDocument(_filePath);
	_cfgDoc->Parse(cfg);
}