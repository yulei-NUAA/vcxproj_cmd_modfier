#include "wildcharmach.h"

#include <ctype.h>

/** Defines and Macros */
#define MATCH     1
#define NOT_MATCH 0

/* 匹配一个字符的宏 */
#define MATCH_CHAR(c1,c2,ignore_case) ( (c1==c2) || ((ignore_case==1) &&(tolower(c1)==tolower(c2))) )

/*  通配符匹配算法
*     src  字符串
*     pattern  含有通配符( * 或 ? 号)的字符串
*     ignore_case 是否区分大小写，1 表示不区分,  0 表示区分
*
*  返回1表示 src 匹配 pattern，返回0表示不匹配
*/
int WildCharMatch(char *src, char *pattern, int ignore_case)
{
	int result;
	while (*src)
	{
		if (*pattern == '*')
		{ /* 如果 pattern 的当前字符是 '*' */
			/* 如果后续有多个 '*', 跳过 */
			while ((*pattern == '*') || (*pattern == '?'))
				pattern++;

			/* 如果 '*" 后没有字符了，则正确匹配 */
			if (!*pattern) return MATCH;

			/* 在 src 中查找一个与 pattern中'*"后的一个字符相同的字符*/
			while (*src && (!MATCH_CHAR(*src, *pattern, ignore_case)))
				src++;

			/* 如果找不到，则匹配失败 */
			if (!*src) return NOT_MATCH;

			/* 如果找到了，匹配剩下的字符串*/
			result = WildCharMatch(src, pattern, ignore_case);
			/* 如果剩下的字符串匹配不上，但src后一个字符等于pattern中'*"后的一个字符 */
			/* src前进一位，继续匹配 */
			while ((!result) && (*(src + 1)) && MATCH_CHAR(*(src + 1), *pattern, ignore_case))
				result = WildCharMatch(++src, pattern, ignore_case);
			return result;

		}
		else
		{
			/* 如果pattern中当前字符不是 '*' */
			/* 匹配当前字符*/
			if (MATCH_CHAR(*src, *pattern, ignore_case) || ('?' == *pattern))
			{
				/* src,pattern分别前进一位，继续匹配 */
				return WildCharMatch(++src, ++pattern, ignore_case);
			}
			else
			{
				return NOT_MATCH;
			}
		}
	}


	/* 如果src结束了，看pattern有否结束*/
	if (*pattern)
	{
		/* pattern没有结束*/
		if ((*pattern == '*') && (*(pattern + 1) == 0)) /* 如果pattern有最后一位字符且是'*' */
			return MATCH;
		else
			return NOT_MATCH;
	}
	else
		return MATCH;
}

