#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef linux
#include <unistd.h>
#include <dirent.h>
#endif

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include "wildcharmach.h"
#endif

#include "GetFileList.h"

static int checkFileExtension(const char* filename, const char * patten)
{
	return WildCharMatch(filename, patten, 1);
}


/**
* @function: 获取search_dir目录下的所有文件名
* @param1:     - 要搜索的目录文件(绝对路径)
* @param2:    patten - 搜索模式 "*" 或者 "*.doc" 或者 "*.c*"
* @param3：   filelist[][MAX_FILE_STR_LENGTH] - 文件名存储路径
* @param4：   find_subdir - 子目录搜索开关
* @param5:    file_num - 文件数量，迭代时传入用于按顺序存储文件名，初始调用时传入0
* @return1:  找到的文件数量 file_num
* 注意，当patten不为*, 且find_subdir==1时会搜索所有文件，然后交给调用get_file_list的函数进行处理
*/
static int get_file_list(char *search_dir,char * patten, char filelist[][MAX_FILE_STR_LENGTH], char find_subdir,int file_num)
{

#ifdef WIN32 
	char cate_dir[MAX_FILE_STR_LENGTH];
	struct _finddata_t file;
	long lf;
	if (find_subdir == 1)
	{	// 先查所有文件，然后自己按照patten过滤
		sprintf(cate_dir, "%s%s", search_dir, "*");
		if ((lf = _findfirst(cate_dir, &file)) == -1)
		{
			_findclose(lf);
			printf("find no files with patten %s in %s\n", patten, search_dir);
			return file_num;
		}
		else
		{
			
			do {
				//如果文件名是 . 或者 .. 则不是具体的文件名，不需要
				if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
					continue;

				//如果文件是目录，则递归调用本函数
				if (file.attrib == _A_SUBDIR)
				{
					char sub_search_dir[MAX_FILE_STR_LENGTH];
					int tmp_file_num;
					sprintf(sub_search_dir, "%s%s\\", search_dir, file.name);
					tmp_file_num = get_file_list(sub_search_dir, "*", filelist, find_subdir, file_num);
					file_num = tmp_file_num;
					if (file_num >= SUPPORT_MAX_FILE_NUM)
					{
						//printf("reached the max file_number！\n");//避免重复打印
						break;
					}
				}
				else
				{
					if (file_num >= SUPPORT_MAX_FILE_NUM)
					{
						printf("reached the max supported file_number which defined by SUPPORT_MAX_FILE_NUM is %d\n", SUPPORT_MAX_FILE_NUM);
						break;
					}
					if (file.attrib != _A_SUBDIR)
					{
						strcat(strcat(filelist[file_num], search_dir), file.name);
						file_num++;
					}
				}

			} while (_findnext(lf, &file) == 0);
		}
	}
	else//不查找子目录，则直接搜索即可
	{
		sprintf(cate_dir, "%s%s", search_dir, patten);
		if ((lf = _findfirst(cate_dir, &file)) == -1)//没有找到，则返回空
		{
			_findclose(lf);
			printf("find no files with patten %s in %s\n", patten, search_dir);
			return file_num;
		}
		else
		{
			do {
				//如果文件名是 . 或者 .. 则不是具体的文件名，不需要
				if (strcmp(file.name, ".") == 0 || strcmp(file.name, "..") == 0)
					continue;

				//如果文件是非目录文件，则存储该文件名到字符串数组中
				if (file.attrib != _A_SUBDIR)
				{
					if (file_num >= SUPPORT_MAX_FILE_NUM)
					{
						printf("reached the max supported file_number which defined by SUPPORT_MAX_FILE_NUM is %d\n", SUPPORT_MAX_FILE_NUM);
						break;
					}
					strcat(strcat(filelist[file_num], search_dir), file.name);
					file_num++;
				}
			} while (_findnext(lf, &file) == 0);
		}
	}
	_findclose(lf);

#endif

#ifdef linux
	DIR *dir;
	struct dirent *ptr;
	char base[1000];

	if ((dir = opendir(search_dir.c_str())) == NULL)
	{
		perror("Open dir error...");
		exit(1);
	}

	while ((ptr = readdir(dir)) != NULL)
	{
		if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)    ///current dir OR parrent dir
			continue;
		else if (ptr->d_type == 8)    ///file
									  //printf("d_name:%s/%s\n",basePath,ptr->d_name);
			files.push_back(ptr->d_name);
		else if (ptr->d_type == 10)    ///link file
									   //printf("d_name:%s/%s\n",basePath,ptr->d_name);
			continue;
		else if (ptr->d_type == 4)    ///dir
		{
			files.push_back(ptr->d_name);
			/*
			memset(base,'\0',sizeof(base));
			strcpy(base,basePath);
			strcat(base,"/");
			strcat(base,ptr->d_nSame);
			readFileList(base);
			*/
		}
	}
	closedir(dir);
#endif

	//排序，按从小到大排序
	/*sort(files.begin(), files.end());*/
	return file_num;
}



/**
* @function: 获取search_dir目录和子目录下的所有文件名,包含绝对路径
* @param1:     - 要搜索的目录文件(绝对路径)
* @param2:    patten - 搜索模式 "*" 或者 "*.doc" 或者 "*.c*"
* @param3：   filelist[][MAX_FILE_STR_LENGTH] - 文件名存储路径
* @return1:  找到的文件数量 file_num 真正的文件数量
*/
int getFileList(char *search_dir, char * patten, char filelist[][MAX_FILE_STR_LENGTH], char find_subdir)
{
	int mathch_file_number = 0;
	if (find_subdir == 0)
	{
		return get_file_list(search_dir, patten, filelist, find_subdir, 0);
	}
	else
	{
		char all_filelist[SUPPORT_MAX_FILE_NUM][MAX_FILE_STR_LENGTH] = { { 0 } };
		int all_file_number = get_file_list(search_dir, "*", all_filelist, find_subdir, 0);
		int i = 0;
		for (i = 0; i < all_file_number; i++)
		{
			if (WildCharMatch(all_filelist[i],patten,1) == 1)
			{
				memcpy(filelist[mathch_file_number], all_filelist[i], strlen(all_filelist[i]));
				mathch_file_number++;
			}
		}
	}
	return mathch_file_number;
}

/**
* @function: 获取search_dir目录下的所有文件名，只包含子目录的路径和文件名
* @param1:   要搜索的目录文件(绝对路径)
* @param2:   patten - 搜索模式 "*" 或者 "*.doc" 或者 "*.c*"
* @param3：  filelist[][MAX_FILE_STR_LENGTH] - 文件名存储路径
* @return1:  找到的文件数量 file_num 真正的文件数量
*/
int getFileNameList(char *search_dir, char * patten, char filelist[][MAX_FILE_STR_LENGTH], char find_subdir)
{
	char tmp_filelist[SUPPORT_MAX_FILE_NUM][MAX_FILE_STR_LENGTH] = { {0} };
	int file_number = getFileList(search_dir, patten, tmp_filelist, find_subdir);
	int i;
	int search_dir_len = strlen(search_dir);

	for (i = 0; i < file_number; i++)
	{
		char *tmp_p = &tmp_filelist[i][search_dir_len];
		memcpy(filelist[i], tmp_p, strlen(tmp_filelist[i])- search_dir_len);
	}
	return file_number;
}
