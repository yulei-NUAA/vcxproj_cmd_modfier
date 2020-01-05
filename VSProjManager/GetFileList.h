#ifndef __GET_FILE_LIST_H__
#define __GET_FILE_LIST_H__

#define SUPPORT_MAX_FILE_NUM 1000  //支持的最大文件数量
#define MAX_FILE_STR_LENGTH  260   //最大的文件路径和文件名长度

#ifdef __cplusplus
extern "C"{
#endif
	/**
	* @function: 获取search_dir目录和子目录下的所有文件名,包含绝对路径
	* @param1:     - 要搜索的目录文件(绝对路径)
	* @param2:    patten - 搜索模式 "*" 或者 "*.doc" 或者 "*.c*"
	* @param3：   filelist[][260] - 文件名存储路径
	* @return1:  找到的文件数量 file_num 真正的文件数量
	*/
	int getFileList(char *search_dir, char * patten, char filelist[][MAX_FILE_STR_LENGTH], char find_subdir);


	/**
	* @function: 获取search_dir目录下的所有文件名，只包含文件名
	* @param1:   要搜索的目录文件(绝对路径)
	* @param2:   patten - 搜索模式 "*" 或者 "*.doc" 或者 "*.c*"
	* @param3：  filelist[][260] - 文件名存储路径
	* @return1:  找到的文件数量 file_num 真正的文件数量
	*/
	int getFileNameList(char *search_dir, char * patten, char filelist[][MAX_FILE_STR_LENGTH], char find_subdir);


#ifdef __cplusplus
}
#endif


#endif // __GET_FILE_LIST
