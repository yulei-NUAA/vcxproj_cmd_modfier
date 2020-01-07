/*******************************************************************
 * filename : GetFileList.h
 * author   : yulei@nuaa.edu.cn
 * content  : 
 * date     : 
/*******************************************************************/


#ifndef __GET_FILE_LIST_H__
#define __GET_FILE_LIST_H__

#define SUPPORT_MAX_FILE_NUM 1000  //支持的最大文件数量
#define MAX_FILE_STR_LENGTH  260   //最大的文件路径和文件名长度

#ifdef __cplusplus
extern "C"{
#endif

int getFileList(char *search_dir, char * patten, char filelist[][MAX_FILE_STR_LENGTH], char find_subdir);
int getFileNameList(char *search_dir, char * patten, char filelist[][MAX_FILE_STR_LENGTH], char find_subdir);


#ifdef __cplusplus
}
#endif


#endif // __GET_FILE_LIST
