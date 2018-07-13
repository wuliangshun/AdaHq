#ifndef SERVER_H_H       
#define SERVER_H_H   

#include <WinSock2.h>

#include <queue>
#include <string>
#include <vector>
#include <map>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#pragma comment(lib, "ws2_32.lib")  
#pragma warning(disable:4996)
#define BUFFERLEN 1024


DWORD WINAPI  sendquotes(LPVOID lpParamter);

DWORD WINAPI  middleServer(LPVOID lpParamter);


#endif