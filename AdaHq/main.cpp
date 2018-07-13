#include "stdafx.h"
#include "server.h"
#include "shhq.h"
#include "szhq.h"

#include <conio.h>

int mdPt = 5099;

char file_dir[50] = "\\\\192.168.0.220\\hq";//C:\\HQ


DWORD WINAPI  shhq(LPVOID lpParamter)
{	
	int last_read_sec = 0;
	WIN32_FILE_ATTRIBUTE_DATA    attr;

	char path[200] = "";
	strcat(path, file_dir);
	strcat(path, "\\shhq\\mktdt00.txt");

	TCHAR file[500];
	#ifdef UNICODE
		MultiByteToWideChar(CP_ACP, 0, path, -1, file, 100);
	#else
		strcpy(file, path);
	#endif   
		
		
	FILETIME modifyTime;
	SYSTEMTIME time;
	int last_write_sec;
	while (true)
	{		
		GetFileAttributesEx(file, GetFileExInfoStandard, &attr);       
		modifyTime = attr.ftLastWriteTime;
		                                                    
		FileTimeToSystemTime(&modifyTime, &time);          
		
		last_write_sec = time.wSecond;
		if (last_write_sec != last_read_sec)
		{			
			printf("shhq:%02d:%02d:%02d\n", (time.wHour + 8), time.wMinute, time.wSecond);
			last_read_sec = last_write_sec;
			//shhq::read_mktdt00();
		}		
	}
	return 0;
}

DWORD WINAPI  szhq(LPVOID lpParamter)
{		
	int last_read_sec = 0;
	WIN32_FILE_ATTRIBUTE_DATA    attr;

	char path[200] = "";
	strcat(path, file_dir);
	strcat(path, "\\szhq\\sjshq.dbf");	

	TCHAR file[50];
	#ifdef UNICODE
		MultiByteToWideChar(CP_ACP, 0, path, -1, file, 100);
	#else
		strcpy(file, path);
	#endif
		
	FILETIME modifyTime;
	SYSTEMTIME time;
	int last_write_sec;
	while (true)
	{		
		GetFileAttributesEx(file, GetFileExInfoStandard, &attr);      	
		modifyTime = attr.ftLastWriteTime;
		time;                                               
		FileTimeToSystemTime(&modifyTime, &time);        

		last_write_sec = time.wSecond;		

		if (last_write_sec != last_read_sec)
		{
			printf("szhq:%02d:%02d:%02d\n", (time.wHour + 8), time.wMinute, time.wSecond);
			last_read_sec = last_write_sec;
			szhq::read_sjshq_dbf();
		}		
	}
	return 0;	
}





int main(int argc, char* argv[])
{		
	if (argc > 1)
	{		
		mdPt = atoi((const char*)argv[1]);		
	}
	if (argc > 2)
	{		
		strcpy(file_dir, argv[2]);
	}
	printf("\nset port = %d\n", mdPt);
	printf("\nset file_dir = %s\n", file_dir);

	//上证行情监测线程	
	HANDLE shhqThread = CreateThread(NULL, 0, shhq, NULL, 0, NULL);
	CloseHandle(shhqThread);
	printf("create shhq thread......\n");

	//深证行情监测线程	
	HANDLE szhqThread = CreateThread(NULL, 0, szhq, NULL, 0, NULL);
	CloseHandle(szhqThread);
	printf("create szhq thread......\n");

	//行情转发线程
	HANDLE serverThread = CreateThread(NULL, 0, middleServer, NULL, 0, NULL);
	CloseHandle(serverThread);
	printf("create server thread......\n");	

	while (getch() != '\n');
	return 0;
}

