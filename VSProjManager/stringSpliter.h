#ifndef __STRING_SPLITER_H__
#define __STRING_SPLITER_H__

#include<iostream>
#include<string>
#include<vector>

std::vector<std::string> splitWithStl(const std::string &str, const std::string &pattern);
std::vector<std::string> split(const std::string &str, const std::string &pattern);
#endif