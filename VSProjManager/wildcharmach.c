#include "wildcharmach.h"

#include <ctype.h>

/** Defines and Macros */
#define MATCH     1
#define NOT_MATCH 0

/* ƥ��һ���ַ��ĺ� */
#define MATCH_CHAR(c1,c2,ignore_case) ( (c1==c2) || ((ignore_case==1) &&(tolower(c1)==tolower(c2))) )

/*  ͨ���ƥ���㷨
*     src  �ַ���
*     pattern  ����ͨ���( * �� ? ��)���ַ���
*     ignore_case �Ƿ����ִ�Сд��1 ��ʾ������,  0 ��ʾ����
*
*  ����1��ʾ src ƥ�� pattern������0��ʾ��ƥ��
*/
int WildCharMatch(char *src, char *pattern, int ignore_case)
{
	int result;
	while (*src)
	{
		if (*pattern == '*')
		{ /* ��� pattern �ĵ�ǰ�ַ��� '*' */
			/* ��������ж�� '*', ���� */
			while ((*pattern == '*') || (*pattern == '?'))
				pattern++;

			/* ��� '*" ��û���ַ��ˣ�����ȷƥ�� */
			if (!*pattern) return MATCH;

			/* �� src �в���һ���� pattern��'*"���һ���ַ���ͬ���ַ�*/
			while (*src && (!MATCH_CHAR(*src, *pattern, ignore_case)))
				src++;

			/* ����Ҳ�������ƥ��ʧ�� */
			if (!*src) return NOT_MATCH;

			/* ����ҵ��ˣ�ƥ��ʣ�µ��ַ���*/
			result = WildCharMatch(src, pattern, ignore_case);
			/* ���ʣ�µ��ַ���ƥ�䲻�ϣ���src��һ���ַ�����pattern��'*"���һ���ַ� */
			/* srcǰ��һλ������ƥ�� */
			while ((!result) && (*(src + 1)) && MATCH_CHAR(*(src + 1), *pattern, ignore_case))
				result = WildCharMatch(++src, pattern, ignore_case);
			return result;

		}
		else
		{
			/* ���pattern�е�ǰ�ַ����� '*' */
			/* ƥ�䵱ǰ�ַ�*/
			if (MATCH_CHAR(*src, *pattern, ignore_case) || ('?' == *pattern))
			{
				/* src,pattern�ֱ�ǰ��һλ������ƥ�� */
				return WildCharMatch(++src, ++pattern, ignore_case);
			}
			else
			{
				return NOT_MATCH;
			}
		}
	}


	/* ���src�����ˣ���pattern�з����*/
	if (*pattern)
	{
		/* patternû�н���*/
		if ((*pattern == '*') && (*(pattern + 1) == 0)) /* ���pattern�����һλ�ַ�����'*' */
			return MATCH;
		else
			return NOT_MATCH;
	}
	else
		return MATCH;
}

