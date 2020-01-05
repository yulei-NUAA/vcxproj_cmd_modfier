/************************************************************************/
/*	filename : main.cpp
	author   : yulei@nuaa.edu.cn
	content  : main func of VSProjManager
	date     : 2019-12-29
*/
/************************************************************************/

#include <iostream>
#include <string>
#include <vector>
#include <codecvt>
#include <ctime>

#include <Windows.h>
#include <wchar.h>

#include "tinyxml.h"
#include "GetFileList.h"
#include "stringSpliter.h"
#include "vsproj_operator.h"
#include "wildcharmach.h"

#include <direct.h>
#define APPLICATION_NAME " Visual Studio Project Manager in Comman Line"
#define VSERSION_NUMBER "v100"
#define VCPROJ_FILE_EXTENSION ".vcxproj"
#define VCPROJ_FILTER_EXTENSION VCPROJ_FILE_EXTENSION + ".filters"
#define BACKUP_FILE_EXTENSION ".bak"
using namespace std;

std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

//Global Value Definetion
string proj_name = "";      /* 要处理的工程名称 */
string directory = "";      /* 待添加文件所在的目录 */
string filter_name = "";    /* 过滤器名称（即工程中资源列表的文件夹名称） */
vector<string> file_list;   /* 待添加到工程的文件列表 */
bool search_sub_directory = false;  /* 是否搜索子目录的开关 */
bool back_up = true;        /* 是否备份原工程的开关 */
vector<string> type_list;   /* 待添加的文件类型列表 */
bool check_flag = false;            /*  */
bool empty_flag = false;            /* 执行插入操作前先清空目录 */

vector<string> filepath_list;//搜索得到的待添加文件

TiXmlDocument vsproj_xml; /* 用于保存vsproj文件的xml对象 */
TiXmlDocument filter_xml; /* 用于保存vsproj.filter文件的xml对象 */
TiXmlElement *vsproj_itemgroup1;/* 保存ProjectConfiguration */
TiXmlElement *vsproj_itemgroup2;/* 保存ClCompile */
TiXmlElement *vsproj_itemgroup3;/* 保存ClInclude */
TiXmlElement *filter_itemgroup1;/* 保存Filter */
TiXmlElement *filter_itemgroup2;/* 保存ClCompile，而且子节点中存有filter的名称 */
TiXmlElement *filter_itemgroup3;/* 保存ClInclude，而且子节点中存有filter的名称 */

void PrintHelp()
{
	cout << "==================== VS Proj Manager Help Info ====================" << endl;
}

void BackupProjFile(string file_name)
{
	string vcxproj = file_name + VCPROJ_FILE_EXTENSION;
	string proj_filters = file_name + VCPROJ_FILTER_EXTENSION;

	time_t now = time(0);
	tm *ltm = localtime(&now);
	
	//ltm->tm_mon + ltm->tm_mday + "_" + ltm->tm_hour + ltm->tm_min + ltm->tm_sec;
	
	string year_str = to_string(ltm->tm_year + 1900);
	string mon_str = to_string(1 + ltm->tm_mon);
	string day_str = to_string(ltm->tm_mday);
	string hour_str = to_string(ltm->tm_hour);
	string min_str = to_string(ltm->tm_min);
	string sec_str = to_string(ltm->tm_sec);
	string time_str = "_"+year_str + mon_str + day_str + "_" + hour_str + min_str + sec_str;

	string cmd = "@copy " + vcxproj + " " + vcxproj+ time_str + BACKUP_FILE_EXTENSION;
	if (system(cmd.c_str()))
	{
		cout << "Cann't found " << vcxproj << ", please check the path and proj_name." << endl;
	}
	cmd = "@copy " + proj_filters + " " + proj_filters+ time_str + BACKUP_FILE_EXTENSION;
	if (system(cmd.c_str()))
	{
		cout << "Cann't found " << proj_filters << ", please check the path and proj_name." << endl;
	}
}

vector<string> GetSearchFilelist(string search_dir, vector<string> cata_type, char find_subdir)
{
	vector<string> search_result;
	char current_address[MAX_FILE_STR_LENGTH] = { 0 };
	char file_list[SUPPORT_MAX_FILE_NUM][MAX_FILE_STR_LENGTH] = { { 0 } };
	int file_num = 0;
	if (search_dir.empty())
	{
		return search_result;
	}
	string full_dir = _getcwd(current_address, MAX_FILE_STR_LENGTH);

	full_dir = full_dir + "\\";

	if (!search_dir.empty())
	{
		full_dir = full_dir + search_dir + "\\"; //获取当前路径
	}
	cout << "search file path is " << full_dir << endl;

	if (cata_type.empty())
	{
		char final_search_dir[260];
		sprintf(final_search_dir, "%s", full_dir.c_str());
		file_num = getFileNameList(final_search_dir, "*", file_list, find_subdir);
		for (int i = 0; i < file_num; i++)
		{
			string tmp_string = file_list[i];
			search_result.push_back(tmp_string);
			//cout << i << ":" << search_result[i] << endl;
			printf("find %03dth file : file name is %s \n", i + 1, tmp_string);
		}
	}
	else
	{
		for each (string cata in cata_type)
		{
			char final_search_dir[260];
			char pattern[260];
			int result_num;
			sprintf(final_search_dir, "%s", full_dir.c_str());
			sprintf(pattern, "*.%s", cata.c_str());
			memset(&file_list[0][0], 0, SUPPORT_MAX_FILE_NUM*MAX_FILE_STR_LENGTH);
			result_num = getFileNameList(final_search_dir, pattern, file_list, find_subdir);
			for (int i = 0; i < result_num; i++)
			{
				string tmp_string = file_list[i];
				search_result.push_back(tmp_string);
				/*cout << "find " << file_num + i + 1 << "th file :" << search_result[i] << endl;*/
				printf("find %03dth file : file name is %s \n", file_num + i + 1, tmp_string.c_str());
			}
			file_num += result_num;
		}
	}
	return search_result;
}




bool options(int count, char* arg[])
{

	int i = 0;
	bool result = true;
	if (count == 1)
	{
		PrintHelp();
		exit(0);
	}
	cout.setf(ios_base::fixed);

	for (i = 1; i < count; i++)
	{
		string argument = string(arg[i]);
		string keyword(argument);
		string value("");
		string::size_type n = argument.find("=");

		if (n != string::npos && n > 0)
		{
			keyword = argument.substr(0, n);
			value = argument.substr(n + 1);
		}

		if (keyword == "--help" || keyword == "-h")
		{
			PrintHelp();
			exit(0);
		}
		else if (keyword == "--version" || keyword == "-v")
		{
			cout << endl << "Application Name : " << APPLICATION_NAME << endl;
			cout << "Version Number : " <<VSERSION_NUMBER << endl;
			exit(0);
		}
		else if (keyword == "--proj_name" || keyword == "-p")
		{
			cout << "set target project name as "<< value << endl;
			proj_name = value;
		}
		else if (keyword == "--directory" || keyword == "-d")
		{
			cout << "set the search directory as " << value << endl;
			directory = value;
		}
		else if (keyword == "--file_name" || keyword == "-f")
		{
			cout <<"files:"<< value << "will be add to project" << endl;
			file_list = splitWithStl(value, ",");
		}
		else if (keyword == "--sub_directory" || keyword == "-s")
		{
			search_sub_directory = true;
			cout << "VSProjManager will search sub directory" << endl;
		}
		else if (keyword == "--no_bakup" || keyword == "-nb")
		{
			cout << "warning: operation without backup file before, this will be dangerous! " << endl;
			back_up = false;
		}
		else if (keyword == "--file_type" || keyword == "-t")
		{
			//cout << "warning: this operation is not supported yet!" << endl;
			type_list = split(value, ",");
		}
		else if (keyword == "--check" || keyword == "-c")
		{
			check_flag = true;
			cout << "just check, no file write" << endl;
		}
		else if (keyword == "--empty_filter" || keyword == "-e")
		{
			cout << "empty filter before insert new file" << endl;
			empty_flag = true;
		}
		else if (keyword == "--filter_name" || keyword == "-fi")
		{
			cout << "target filter name is "<< value << endl;
			filter_name = value;
		}
	}

	//输入参数错误检查
	//proj_name的value不能为空
	if (proj_name.empty())
	{
		cout << "proj_name cann't be empty!" << endl;
		return false;
	}
	//filter_name的value不能为空
	if (filter_name.empty())
	{
		cout << "filter_name cann't be empty!" << endl;
		return false;
	}
	//directory和filename不能同时为空
	if (directory.empty() && file_list.empty())
	{
		cout << "directory and file_list cann't simultaneous be empty !" << endl;
		return false;
	}

	if (!proj_name.empty())
	{
		string filename = "";
		string filterfilename = "";

		filename = proj_name + VCPROJ_FILE_EXTENSION;
		filterfilename = proj_name + VCPROJ_FILTER_EXTENSION;

		bool openproj = vsproj_xml.LoadFile(filename.c_str(), TIXML_ENCODING_UTF8);
		bool openfilter = filter_xml.LoadFile(filterfilename.c_str(), TIXML_ENCODING_UTF8);
		if (openproj && openfilter)
		{
			cout << "load file '" << filename << "' and '" << filterfilename << "' succeed!" << endl;
		}
		else
		{
			cout << "load file '" << filename << "' and '" << filterfilename << "' failed!" << endl;
			cout << "please check the file path" << endl;
			result = false;
		}
	}

	vsproj_itemgroup1 = getTiXmlGroup(&vsproj_xml,"ItemGroup",1);/* 保存ProjectConfiguration */
	vsproj_itemgroup2 = getTiXmlGroup(&vsproj_xml, "ItemGroup", 2);/* 保存ClCompile */
	vsproj_itemgroup3 = getTiXmlGroup(&vsproj_xml, "ItemGroup", 3);;/* 保存ClInclude */
	filter_itemgroup1 = getTiXmlGroup(&filter_xml, "ItemGroup", 1);;/* 保存Filter */
	filter_itemgroup2 = getTiXmlGroup(&filter_xml, "ItemGroup", 2);;/* 保存ClCompile，而且子节点中存有filter的名称 */
	filter_itemgroup3 = getTiXmlGroup(&filter_xml, "ItemGroup", 3);;/* 保存ClInclude，而且子节点中存有filter的名称 */
	
	return result;
}
 

vector<string> getCompileFileList(vector<string> file_list, int compile_able)
{
	vector<string> result;
	char str[260];

	for each (string each__name in file_list)
	{
		if (compile_able)//检查后缀为*.c *.cpp *.cxx的文件
		{
			strcpy(str, each__name.c_str());
			if (WildCharMatch(str, "*.c", 1) == 1)
			{
				result.push_back(each__name);
			}
			else if (WildCharMatch(str, "*.cpp", 1) == 1)
			{
				result.push_back(each__name);
			}
			else if (WildCharMatch(str, "*.cxx", 1) == 1)
			{
				result.push_back(each__name);
			}
		}
		else //检查后缀为*.h *.hpp *.hxx
		{
			strcpy(str, each__name.c_str());
			if (WildCharMatch(str, "*.h", 1) == 1)
			{
				result.push_back(each__name);
			}
			else if (WildCharMatch(str, "*.hpp", 1) == 1)
			{
				result.push_back(each__name);
			}
			else if (WildCharMatch(str, "*.hxx", 1) == 1)
			{
				result.push_back(each__name);
			}
		}
	}
	return result;
}



void real_main(int argc, char*argv[])
{
	bool param_proc_success;

	// *** INIT OPTIONS VALUES OR FLAGS FOR THIS APPLICATION ***//
	proj_name = "";      /* 要处理的工程名称 */
	directory = "";      /* 待添加文件所在的目录 */
	file_list.clear();   /* 待添加到工程的文件列表 */
	search_sub_directory = false;  /* 是否搜索子目录的开关 */
	back_up = true;        /* 是否备份原工程的开关 */
	type_list.clear();   /* 待添加的文件类型列表 */
	check_flag = false;            /*  */
	empty_flag = false;            /* 执行插入操作前先清空目录 */
	filepath_list.clear();  /*搜索得到的待添加文件*/

	// *** PARSE OPTIONS PASSED INTO THIS SPECIFIC APPLICATION: VPM *** //
	param_proc_success = options(argc, argv);
	if (!param_proc_success) 
	{
		PrintHelp();
		exit(-1);
	}

	// *** 根据输入参数执行文件添加的操作 *** //
	// 根据需要备份当前工程文件
	if (back_up)
	{
		BackupProjFile(proj_name);
	}
	else
	{
		cout << "Operation with out backup..." << endl;
		//cout << "出错了可不负责哦！" << endl;
	}
	
	// 获取完整的待添加文件列表
	filepath_list = GetSearchFilelist(directory,type_list,1);
	

	// 根据需要清空指定filter中的文件
	if (empty_flag)
	{
		vector<string> cl_file_to_rm;
		vector<string> inc_file_to_rm;
		//filter_xml 遍历 3个group,匹配filter名称并进行删除，同时获取待删除文件列表
		if (!FilterExist(filter_itemgroup1,filter_name))
		{
			cout << "filter " << filter_name << " does not exist! please check the filter name!" << endl;
			exit(-1);
		}
		cl_file_to_rm = RemoveFileNodeByFilter(filter_itemgroup2,filter_name);/* 保存ClCompile，而且子节点中存有filter的名称 */
		inc_file_to_rm = RemoveFileNodeByFilter(filter_itemgroup3, filter_name);/* 保存ClInclude，而且子节点中存有filter的名称 */

		//vsproj_xml; 遍历2个group 根据待删除文件列表删除相应节点
		RemoveFileNodeByFile(vsproj_itemgroup2,cl_file_to_rm);/* ClCompile */
		RemoveFileNodeByFile(vsproj_itemgroup3, inc_file_to_rm); ;/* ClInclude */
		
	}

	// 对待添加文件列表进行分类：可编译文件、包含文件
	file_list.insert(file_list.end(),filepath_list.begin(),filepath_list.end());
	vector<string> clc_list = getCompileFileList(file_list,1);//获取可以编译的文件列表
	vector<string> inc_list = getCompileFileList(file_list,0);//获取不可编译的文件列表

	// 进行文件插入操作
	InsertFileToXml(filter_itemgroup2, clc_list, "ClCompile", filter_name);
	InsertFileToXml(filter_itemgroup3, inc_list, "ClInclude", filter_name);
	InsertFileToXml(vsproj_itemgroup2, clc_list, "ClCompile", "");//插入vsproj XML时不需要传入filtername
	InsertFileToXml(vsproj_itemgroup3, inc_list, "ClInclude", "");
	// 保存文件
	if (!check_flag)
	{
		if (vsproj_xml.SaveFile() && filter_xml.SaveFile())
		{
			cout << "proj files saved succeed!" <<endl;
		}
	}
}

int main(int argc, char * argv[])
{
	try {
		real_main(argc, argv);
	}
	catch (string& msg) {
		std::cerr << "FATAL ERROR: VSProjManager terminated with an exception."
			<< std::endl << "The message was: " << msg << std::endl;
		return 1;
	}
	catch (...) {
		std::cerr << "FATAL ERROR: VSProjManager terminated with an unknown exception."
			<< std::endl;
		return 1;
	}
	return 0;
}
