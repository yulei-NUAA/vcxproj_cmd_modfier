/************************************************************************
 * filename : main.cpp
 * author   : yulei@nuaa.edu.cn
 * content  : main func of VSProjManager
 * date     : 2019-12-29
 * 
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
string proj_name = "";      /* Ҫ����Ĺ������� */
string directory = "";      /* ������ļ����ڵ�Ŀ¼ */
string filter_name = "";    /* ���������ƣ�����������Դ�б���ļ������ƣ� */
vector<string> file_list;   /* ����ӵ����̵��ļ��б� */
bool search_sub_directory = false;  /* �Ƿ�������Ŀ¼�Ŀ��� */
bool back_up = true;        /* �Ƿ񱸷�ԭ���̵Ŀ��� */
vector<string> type_list;   /* ����ӵ��ļ������б� */
bool check_flag = false;            /*  */
bool empty_flag = false;            /* ִ�в������ǰ�����Ŀ¼ */

vector<string> filepath_list;//�����õ��Ĵ�����ļ�

TiXmlDocument vsproj_xml; /* ���ڱ���vsproj�ļ���xml���� */
TiXmlDocument filter_xml; /* ���ڱ���vsproj.filter�ļ���xml���� */
TiXmlElement *vsproj_itemgroup1;/* ����ProjectConfiguration */
TiXmlElement *vsproj_itemgroup2;/* ����ClCompile */
TiXmlElement *vsproj_itemgroup3;/* ����ClInclude */
TiXmlElement *filter_itemgroup1;/* ����Filter */
TiXmlElement *filter_itemgroup2;/* ����ClCompile�������ӽڵ��д���filter������ */
TiXmlElement *filter_itemgroup3;/* ����ClInclude�������ӽڵ��д���filter������ */

void PrintHelp()
{
	//cout << "==================== VS Proj Manager Help Info ====================" << endl;
	cout << "Usage: VSProjManager -p=proj_name -fi=filter_name [-f=file_name1,file_name2] [option]" << endl;
	cout << "--version       : ";  //��ӡ�汾��Ϣ
	cout << "print the  version info; also as -v" << endl;
	cout << "--help          : ";  //��ӡ������Ϣ
	cout << "print this help message and exit; also as -h" << endl;
	cout << "--proj_name=[]  : ";  //�������� ����ָ��Ҫ�����޸ĵĹ������ƣ����԰�����������·��
	cout << "set the project name; also as -p=[]" << endl;
	cout << "--directory=[]  : ";  //����ӵ��ļ�����Ŀ¼�������Ǿ���Ŀ¼��Ҳ���������Ŀ¼��
	cout << "set the directory of target source file; also as -d=[]" << endl;
	cout << "--filter_name=[]: ";  //���������� ����ָ������ӵ�Ŀ¼���ڵĹ�����·��
	cout << "set the target fileter name in *.vcxproj.filter file; also as -fi=[]" << endl;
	cout << "--file_name=[]  : ";  //����ӵ��ļ��������Ǿ���·����Ҳ���������·����
	cout << "set the target source file list; also as -f=[]" << endl;
	cout << "--sub_directory : ";  //ѭ��������Ŀ¼����ļ�
	cout << "if set, program will search source file in subdir of the target directory; also as -s" << endl;
	cout << "--no_bakup      : ";  //���ݿ���Ĭ�ϴ򿪣��˲�����ر��Զ�����	
	cout << "if set, program will not create bakup files before operation; also as -nb" << endl;
	cout << "--check         : ";  //ֻ���м�飬��ӡ���ᱻ��ӵ��ļ����������ļ�д����
	cout << "if set, program will not write file to disk, just print the search result; also as -c" << endl;
	cout << "--empty_filter  : ";  //���ָ����filter�а������ļ�
	cout << "if set, program will empty the target filter; also as -e" << endl;
	//cout << "--file_type = [](-t=[])   " << endl;  //Ҫ��ӵ��ļ����ͣ�c��cpp��������--file_type = c, cpp, php, xml
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
		full_dir = full_dir + search_dir + "\\"; //��ȡ��ǰ·��
	}
	cout << "search file path is " << full_dir << endl;

	if (cata_type.empty())
	{
		char final_search_dir[260];
		sprintf(final_search_dir, "%s", full_dir.c_str());
		file_num = getFileNameList(final_search_dir, "*", file_list, find_subdir);
		for (int i = 0; i < file_num; i++)
		{
			string tmp_string = search_dir + "\\" + file_list[i];
			search_result.push_back(tmp_string);
			//cout << i << ":" << search_result[i] << endl;
			//printf("find %03dth file : file name is %s \n", i + 1, tmp_string);
			cout << "find " << i + 1 << "th file : file name is " << tmp_string << endl;
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
				string tmp_string = search_dir + "\\" + file_list[i];
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
		// else if (keyword == "--file_type" || keyword == "-t")
		// {
		// 	//cout << "warning: this operation is not supported yet!" << endl;
		// 	type_list = split(value, ",");
		// }
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

	//�������������
	//proj_name��value����Ϊ��
	if (proj_name.empty())
	{
		cout << "proj_name cann't be empty!" << endl;
		return false;
	}
	//filter_name��value����Ϊ��
	if (filter_name.empty())
	{
		cout << "filter_name cann't be empty!" << endl;
		return false;
	}
	//directory��filename����ͬʱΪ��
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

		bool openproj = vsproj_xml.LoadFile(filename.c_str());
		bool openfilter = filter_xml.LoadFile(filterfilename.c_str());
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

	vsproj_itemgroup1 = getTiXmlGroup(&vsproj_xml,"ItemGroup","ProjectConfiguration");/* ����ProjectConfiguration */
	vsproj_itemgroup2 = getTiXmlGroup(&vsproj_xml, "ItemGroup", "ClCompile");/* ����ClCompile */
	vsproj_itemgroup3 = getTiXmlGroup(&vsproj_xml, "ItemGroup", "ClInclude");/* ����ClInclude */
	filter_itemgroup1 = getTiXmlGroup(&filter_xml, "ItemGroup", "Filter");/* ����Filter */
	filter_itemgroup2 = getTiXmlGroup(&filter_xml, "ItemGroup", "ClCompile");/* ����ClCompile�������ӽڵ��д���filter������ */
	filter_itemgroup3 = getTiXmlGroup(&filter_xml, "ItemGroup", "ClInclude");/* ����ClInclude�������ӽڵ��д���filter������ */
	
	return result;
}
 

vector<string> getCompileFileList(vector<string> file_list, string dir, int compile_able)
{
	vector<string> result;
	char str[260];

	for each (string each__name in file_list)
	{
		if (compile_able)//����׺Ϊ*.c *.cpp *.cxx���ļ�
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
		else //����׺Ϊ*.h *.hpp *.hxx
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
	cout << "Input Command is: ";
	for (int i=0;i<argc;i++)
	{
		cout << argv[i];
	}
	cout << endl;
	bool param_proc_success;

	// *** INIT OPTIONS VALUES OR FLAGS FOR THIS APPLICATION ***//
	proj_name = "";      /* Ҫ����Ĺ������� */
	directory = "";      /* ������ļ����ڵ�Ŀ¼ */
	file_list.clear();   /* ����ӵ����̵��ļ��б� */
	search_sub_directory = false;  /* �Ƿ�������Ŀ¼�Ŀ��� */
	back_up = true;        /* �Ƿ񱸷�ԭ���̵Ŀ��� */
	type_list.clear();   /* ����ӵ��ļ������б� */
	check_flag = false;            /*  */
	empty_flag = false;            /* ִ�в������ǰ�����Ŀ¼ */
	filepath_list.clear();  /*�����õ��Ĵ�����ļ�*/

	// *** PARSE OPTIONS PASSED INTO THIS SPECIFIC APPLICATION: VPM *** //
	param_proc_success = options(argc, argv);
	if (!param_proc_success) 
	{
		PrintHelp();
		exit(-1);
	}

	// *** �����������ִ���ļ���ӵĲ��� *** //
	// ������Ҫ���ݵ�ǰ�����ļ�
	if (back_up && !check_flag)
	{
		BackupProjFile(proj_name);
	}
	else
	{
		cout << "Operation with out backup..." << endl;
		//cout << "�����˿ɲ�����Ŷ��" << endl;
	}
	
	// ��ȡ�����Ĵ�����ļ��б�
	filepath_list = GetSearchFilelist(directory,type_list,1);
	

	// ������Ҫ���ָ��filter�е��ļ�
	if (empty_flag)
	{
		vector<string> cl_file_to_rm;
		vector<string> inc_file_to_rm;
		//filter_xml ���� 3��group,ƥ��filter���Ʋ�����ɾ����ͬʱ��ȡ��ɾ���ļ��б�
		if (!FilterExist(filter_itemgroup1,filter_name))
		{
			cout << "filter " << filter_name << " does not exist! please check the filter name!" << endl;
			exit(-1);
		}
		cl_file_to_rm = RemoveFileNodeByFilter(filter_itemgroup2,filter_name);/* ����ClCompile�������ӽڵ��д���filter������ */
		inc_file_to_rm = RemoveFileNodeByFilter(filter_itemgroup3, filter_name);/* ����ClInclude�������ӽڵ��д���filter������ */

		//vsproj_xml; ����2��group ���ݴ�ɾ���ļ��б�ɾ����Ӧ�ڵ�
		RemoveFileNodeByFile(vsproj_itemgroup2,cl_file_to_rm);/* ClCompile */
		RemoveFileNodeByFile(vsproj_itemgroup3, inc_file_to_rm); ;/* ClInclude */
		
	}

	// �Դ�����ļ��б���з��ࣺ�ɱ����ļ��������ļ�
	file_list.insert(file_list.end(),filepath_list.begin(),filepath_list.end());
	vector<string> clc_list = getCompileFileList(file_list, directory, 1);//��ȡ���Ա�����ļ��б�
	vector<string> inc_list = getCompileFileList(file_list, directory, 0);//��ȡ���ɱ�����ļ��б�

	// �����ļ��������
	InsertFileToXml(filter_itemgroup2, clc_list, "ClCompile", filter_name);
	InsertFileToXml(filter_itemgroup3, inc_list, "ClInclude", filter_name);
	InsertFileToXml(vsproj_itemgroup2, clc_list, "ClCompile", "");//����vsproj XMLʱ����Ҫ����filtername
	InsertFileToXml(vsproj_itemgroup3, inc_list, "ClInclude", "");
	// �����ļ�
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
