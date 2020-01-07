#include "stringSpliter.h"

//************************************
// Method:    splitWithStl
// FullName:  splitWithStl
// Access:    public 
// Returns:   std::vector<std::string>
// Qualifier:
// Parameter: const std::string & str
// Parameter: const std::string & pattern
//************************************
std::vector<std::string> splitWithStl(const std::string &str, const std::string &pattern)
{
	std::vector<std::string> resVec;

	if ("" == str)
	{
		return resVec;
	}
	//方便截取最后一段数据
	std::string strs = str + pattern;

	size_t pos = strs.find(pattern);
	size_t size = strs.size();

	while (pos != std::string::npos)
	{
		std::string x = strs.substr(0, pos);
		resVec.push_back(x);
		strs = strs.substr(pos + 1, size);
		pos = strs.find(pattern);
	}

	return resVec;
}

//************************************
// Method:    split
// FullName:  split
// Access:    public 
// Returns:   std::vector<std::string>
// Qualifier:
// Parameter: const std::string & str
// Parameter: const std::string & pattern
//************************************
std::vector<std::string> split(const std::string &str, const std::string &pattern)
{
	//const char* convert to char*
	char * strc = new char[strlen(str.c_str()) + 1];
	strcpy(strc, str.c_str());
	std::vector<std::string> resultVec;
	char* tmpStr = strtok(strc, pattern.c_str());
	while (tmpStr != NULL)
	{
		resultVec.push_back(std::string(tmpStr));
		tmpStr = strtok(NULL, pattern.c_str());
	}

	delete[] strc;

	return resultVec;
}