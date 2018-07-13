#ifndef TOOL_H_H
#define TOOL_H_H


#include <string>
#include <fstream>
#include <vector>

using namespace std;

#pragma warning( disable : 4996)


vector<char*> splitbuff(char* buff, char* d);
char* ltrim_lc(char* s);
#endif 