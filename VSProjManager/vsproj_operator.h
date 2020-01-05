#ifndef __VSPROJ_OPERATOR_H__
#define __VSPROJ_OPERATOR_H__

#include <iostream>
#include <string>
#include <vector>
#include "tinyxml.h"
#include "stringSpliter.h"
#include "vsproj_operator.h"
#include "tinyxml.h"
#include "stringSpliter.h"
using namespace std;

TiXmlElement * getTiXmlGroup(TiXmlDocument* tixml_doc, const char* group_name, int idx);
bool FilterExist(TiXmlElement * itemgroup, string filter_name);
vector<string> RemoveFileNodeByFilter(TiXmlElement * itemgroup, string filter_name);
void RemoveFileNodeByFile(TiXmlElement * itemgroup, vector<string> rmfile_list);
void InsertFileToXml(TiXmlElement * itemgroup, vector<string> file_list, string item_name, string filter_name);


#endif // __VSPROJ_OPERATOR_H__
