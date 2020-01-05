#ifndef __GET_FILE_LIST_H__
#define __GET_FILE_LIST_H__

#define SUPPORT_MAX_FILE_NUM 1000  //֧�ֵ�����ļ�����
#define MAX_FILE_STR_LENGTH  260   //�����ļ�·�����ļ�������

#ifdef __cplusplus
extern "C"{
#endif
	/**
	* @function: ��ȡsearch_dirĿ¼����Ŀ¼�µ������ļ���,��������·��
	* @param1:     - Ҫ������Ŀ¼�ļ�(����·��)
	* @param2:    patten - ����ģʽ "*" ���� "*.doc" ���� "*.c*"
	* @param3��   filelist[][260] - �ļ����洢·��
	* @return1:  �ҵ����ļ����� file_num �������ļ�����
	*/
	int getFileList(char *search_dir, char * patten, char filelist[][MAX_FILE_STR_LENGTH], char find_subdir);


	/**
	* @function: ��ȡsearch_dirĿ¼�µ������ļ�����ֻ�����ļ���
	* @param1:   Ҫ������Ŀ¼�ļ�(����·��)
	* @param2:   patten - ����ģʽ "*" ���� "*.doc" ���� "*.c*"
	* @param3��  filelist[][260] - �ļ����洢·��
	* @return1:  �ҵ����ļ����� file_num �������ļ�����
	*/
	int getFileNameList(char *search_dir, char * patten, char filelist[][MAX_FILE_STR_LENGTH], char find_subdir);


#ifdef __cplusplus
}
#endif


#endif // __GET_FILE_LIST
