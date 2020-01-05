#ifndef __WILD_CHAR_MATCH_H__
#define __WILD_CHAR_MATCH_H__

#define SUPPORT_MAX_FILE_NUM 1000  //֧�ֵ�����ļ�����
#define MAX_FILE_STR_LENGTH  260   //�����ļ�·�����ļ�������

#ifdef __cplusplus
extern "C" {
#endif
	
	/*  ͨ���ƥ���㷨
	*     src  �ַ���
	*     pattern  ����ͨ���( * �� ? ��)���ַ���
	*     ignore_case �Ƿ����ִ�Сд��1 ��ʾ������,  0 ��ʾ����
	*
	*  ����1��ʾ src ƥ�� pattern������0��ʾ��ƥ��
	*/
	int WildCharMatch(char * src, char * pattern, int ignore_case);

#ifdef __cplusplus
}
#endif


#endif // __GET_FILE_LIST


