#include <iostream>
#include <string>
#include <vector>

#include "tinyxml.h"
#include "stringSpliter.h"
#include "vsproj_operator.h"

#include <Windows.h>
#include <wchar.h>

#include "vsproj_operator.h"


//************************************
// Method:    getTiXmlGroup
// FullName:  获取xml文档中一级节点的指针
// Access:    public 
// Returns:   TiXmlElement * 返回group指针
// Qualifier:
// Parameter: TiXmlDocument * tixml_doc 输入的xml文件
// Parameter: const char* group_name 查询的group名称
// Parameter: int idx 指定要获取的是第几个group
//************************************
TiXmlElement * getTiXmlGroup(TiXmlDocument* tixml_doc, const char* group_name, const char * item_type)
{
	TiXmlElement *itemGroupNode;
	if (tixml_doc->FirstChildElement() != NULL)
		itemGroupNode = tixml_doc->FirstChildElement()->FirstChildElement(group_name);
	else
		itemGroupNode = NULL;

	while (itemGroupNode != NULL)
	{
		if (itemGroupNode->FirstChildElement(item_type) == NULL)
		{
			itemGroupNode = (TiXmlElement *)itemGroupNode->NextSiblingElement(group_name);
		}
		else
		{
			break;
		}
		
		
	}

	return itemGroupNode;
}


//************************************
// Method:    FilterExist
// FullName:  FilterExist 根据filter名称判断filter是否存在
// Access:    public 
// Returns:   bool filter是否存在
// Qualifier:
// Parameter: TiXmlElement * itemgroup filter itemgroup
// Parameter: string filter_name 要清空的filter
//************************************
bool FilterExist(TiXmlElement * itemgroup, string filter_name)
{
	//throw std::logic_error("The method or operation is not implemented.");
	WCHAR WideCharFilter[100] = { 0 };
	TiXmlElement *filter_node; 
	if (itemgroup == NULL)
	{
		return false;
	}
	filter_node = itemgroup->FirstChildElement("Filter");
	if (filter_node == NULL)
	{
		return false;
	}
	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, filter_name.c_str(), filter_name.length(), WideCharFilter, sizeof(WCHAR) * 100);

	while (filter_node!=NULL)
	{
		WCHAR WideCharStr[100] = {0};
		string tmp_filter_name = filter_node->Attribute("Include");
		if (tmp_filter_name.empty())
		{
			break;
		}
		MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, tmp_filter_name.c_str(), tmp_filter_name.length(), WideCharStr, sizeof(WCHAR) * 100);
		if (memcmp(WideCharFilter, WideCharStr,sizeof(WCHAR)*100) == 0)
		{
			return true;
		}
		filter_node = filter_node->NextSiblingElement("Filter");
	}
	return false;
}

//************************************
// Method:    RemoveFileNodeByFilter
// FullName:  根据filter名称删除相应的节点
// Access:    public 
// Returns:   std::vector<std::string>
// Qualifier:
// Parameter: TiXmlElement * itemgroup
// Parameter: string filter_name
//************************************
vector<string> RemoveFileNodeByFilter(TiXmlElement * itemgroup, string filter_name)
{
	vector<string> file_list;
	WCHAR WideCharFilter[100] = { 0 };
	TiXmlElement *childNode;
	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, filter_name.c_str(), filter_name.length(), WideCharFilter, sizeof(WCHAR) * 100);

	if (itemgroup!=NULL)
		childNode = itemgroup->FirstChildElement();
	else
	{
		cout << "itemgroup is invalid !" << endl;
		throw std::logic_error("itemgroup is invalid !");
	}
	while (childNode != NULL)
	{
		WCHAR WideCharStr[100] = { 0 };
		string tmp_filter_name;
		TiXmlElement *filterNode = childNode->FirstChildElement("Filter");
		if (filterNode != NULL)
		{
			tmp_filter_name = filterNode->GetText();
			MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, tmp_filter_name.c_str(), tmp_filter_name.length(), WideCharStr, sizeof(WCHAR) * 100);
		}
		if (memcmp(WideCharFilter, WideCharStr, sizeof(WCHAR) * 100) == 0)//filter匹配，则先保存文件名，再删除该节点
		{
			string file_name = childNode->Attribute("Include");
			file_list.push_back(file_name);
			TiXmlElement *nextNode = childNode->NextSiblingElement();
			if (itemgroup->RemoveChild(childNode))
			{
				childNode = nextNode;
			}
			else
			{
				cout << "node delete failed!" << endl;
				throw std::logic_error("Node delete failed!");
				return file_list;
			}
		}
		else
		{
			childNode = childNode->NextSiblingElement();
		}		
	}

	return file_list;
}

//************************************
// Method:    RemoveFileNodeByFile
// FullName:  根据文件名删除相应的节点
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: TiXmlElement * itemgroup
// Parameter: vector<string> rmfile_list
//************************************
void RemoveFileNodeByFile(TiXmlElement * itemgroup, vector<string> rmfile_list)
{
	//throw std::logic_error("The method or operation is not implemented.");
	
	for each(string  filename in rmfile_list)
	{
		TiXmlElement *childNode;
		if (itemgroup != NULL)
		{
			childNode = itemgroup->FirstChildElement();
		}
		else
		{
			cout << "itemgroup is invalid !" << endl;
			throw std::logic_error("itemgroup is invalid !");
		}

		/*cout << endl << filename << endl;*/
		while (childNode!=NULL)
		{
			string file_name_in_xml = childNode->Attribute("Include");
			TiXmlElement *nextNode = childNode->NextSiblingElement();

			if (file_name_in_xml == filename)
			{
				if(itemgroup->RemoveChild(childNode))
				{
					childNode = nextNode;
				}
				else//删除出错
				{
					cout << "delete failed!" << endl;
				}
			}
			childNode = nextNode;
		}
	}
	
}

void ANSI2UTF8(const char * ansi_str, char *utf8_str, int str_len)
{
	BOOL default_char_used;
	WCHAR wstr[260] = { 0 };
	MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, ansi_str, str_len, wstr, sizeof(WCHAR) * str_len);
	WideCharToMultiByte(CP_UTF8, 0, wstr, sizeof(WCHAR)*str_len, utf8_str, 260, 0, &default_char_used);
	if (default_char_used)
	{
		cout << endl;
	}
	return;
}

//************************************
// Method:    InsertFileToXml
// FullName:  InsertFileToXml
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: TiXmlElement * itemgroup
// Parameter: vector<string> file_list
// Parameter: string filter_name
//************************************
void InsertFileToXml(TiXmlElement * itemgroup, vector<string> file_list, string item_name, string filter_name)
{
	//throw std::logic_error("The method or operation is not implemented.");
	if (itemgroup == NULL)
	{
		cout << "itemgroup is invalid !" << endl;
		throw std::logic_error("itemgroup is invalid !");
	}
	for each(string each_name in file_list)
	{
		char utf8_str[260] = { 0 };
		
		TiXmlElement file_node(item_name.c_str());
		
		//TODO:将 each_name.c_str() 转化为utf8字符串utf8_str
		ANSI2UTF8(each_name.c_str(), utf8_str, each_name.length());
		file_node.SetAttribute("Include", utf8_str);
		if (!filter_name.empty())
		{
			TiXmlElement filter_node("Filter");
			//TODO将filter_name转化为字符串utf8_str
			ANSI2UTF8(filter_name.c_str(), utf8_str, filter_name.length());
			TiXmlText filter_text(utf8_str);
			filter_node.InsertEndChild(filter_text);
			file_node.InsertEndChild(filter_node);
		}

		itemgroup->InsertEndChild(file_node);
	}
}