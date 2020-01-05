#ifndef __WILD_CHAR_MATCH_H__
#define __WILD_CHAR_MATCH_H__

#define SUPPORT_MAX_FILE_NUM 1000  //支持的最大文件数量
#define MAX_FILE_STR_LENGTH  260   //最大的文件路径和文件名长度

#ifdef __cplusplus
extern "C" {
#endif
	
	/*  通配符匹配算法
	*     src  字符串
	*     pattern  含有通配符( * 或 ? 号)的字符串
	*     ignore_case 是否区分大小写，1 表示不区分,  0 表示区分
	*
	*  返回1表示 src 匹配 pattern，返回0表示不匹配
	*/
	int WildCharMatch(char * src, char * pattern, int ignore_case);

#ifdef __cplusplus
}
#endif


#endif // __GET_FILE_LIST


