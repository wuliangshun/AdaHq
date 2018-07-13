#include "server.h"

extern int mdPt;//转发端口
vector<SOCKET> sockets;//普通行情客户端
vector<SOCKET> gjSockets;//深度行情客户端
HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
HANDLE sMutex = CreateMutex(NULL, FALSE, NULL);
queue<string> msgqueue;//消息队列


DWORD WINAPI sendquotes(LPVOID lpParamter)
{
	//GetTickCount()调用函数需包含windows.h。得到的是系统运行的时间 精确到毫秒
	//SYSTEMTIME lasttime, nowtime;//GetLocalTime(&lasttime);结构体，精度是秒
	double lasttime, nowtime;
	lasttime = GetTickCount();

	while (1) {
		if (msgqueue.empty())
		{
			nowtime = GetTickCount();
			if (nowtime - lasttime > 3000)//每隔三秒发一个heartbeat信息
			{
				WaitForSingleObject(hMutex, INFINITE);
				string s = "heartbeat;";
				msgqueue.push(s);
				ReleaseMutex(hMutex);
				lasttime = nowtime;
			}
			continue;
		}
		//sprintf(sendBuff, "Message %d is: ok", ncount++);    //将ncount的值放入字符串senBuff中
		//**********************  第三步使用sendto函数进行通信    *************************// 
		WaitForSingleObject(hMutex, INFINITE);
		string s = msgqueue.front();
		msgqueue.pop();
		ReleaseMutex(hMutex);
		char *pPack = (char*)s.data();
		WaitForSingleObject(sMutex, INFINITE);
		vector<SOCKET>::iterator t;
		bool advanced = s[0] == 'o' || s[0] == 't' || s[0] == 'q';
		if (!advanced)//非高级行情才发送给普通订阅者
		{
			for (t = sockets.begin(); t < sockets.end(); t++)//普通行情订阅者
			{
				int iSend = send(*t, pPack, s.length(), 0);//发送数据  
				if (iSend == SOCKET_ERROR){
					printf("\nA client go offline ...\n\n");
					t = sockets.erase(t);
					if (t == sockets.end()) //要控制迭代器不能超过整个容器
					{
						break;
					}
				}
			}
		}
		else //任何行情都发送给高级订阅者
		{
			for (t = gjSockets.begin(); t < gjSockets.end(); t++)//高级行情订阅者
			{
				int iRet = send(*t, pPack, s.length(), 0);
				if (iRet == SOCKET_ERROR)
				{
					printf("\nA client go offline ...\n\n");
					t = gjSockets.erase(t);
					if (t == gjSockets.end())
					{
						break;
					}
				}
			}
		}
		ReleaseMutex(sMutex);
	}
}

//中间服务器
DWORD WINAPI  middleServer(LPVOID lpParamter)
{
	WSADATA wsaData;

	char buf[] = "Server: hello, I am a server.....";

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed to load Winsock");
		return -1;
	}

	//创建用于监听的套接字  
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(mdPt); //1024以上的端口号  
	addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

	int retVal = ::bind(sockSrv, (LPSOCKADDR)&addrSrv, sizeof(SOCKADDR_IN));
	if (retVal == SOCKET_ERROR){
		printf("Failed bind:%d\n", WSAGetLastError());
		return -1;
	}

	if (listen(sockSrv, 10) == SOCKET_ERROR){
		printf("Listen failed:%d", WSAGetLastError());
		return -1;
	}

	SOCKADDR_IN addrClient;
	int len = sizeof(SOCKADDR);

	CreateThread(NULL, 0, sendquotes, NULL, 0, NULL);//启动行情广播线程	

	while (1)
	{
		//等待客户请求到来
		SOCKET sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
		if (sockConn == SOCKET_ERROR){
			printf("Accept failed:%d", WSAGetLastError());
			break;
		}
		//默认添加到普通用户列表
		WaitForSingleObject(sMutex, INFINITE);
		sockets.insert(sockets.end(), sockConn);
		printf("Accept client IP:[%s] as common user\n", inet_ntoa(addrClient.sin_addr));
		ReleaseMutex(sMutex);

		int nNetTimeout = 1000;//1秒，
		//设置发送超时
		//setsockopt(socket，SOL_SOCKET, SO_SNDTIMEO，(char *)&nNetTimeout, sizeof(int));
		//设置接收超时
		//setsockopt(socket，SOL_SOCKET, SO_RCVTIMEO，(char *)&nNetTimeout, sizeof(int));
		setsockopt(sockConn, SOL_SOCKET, SO_RCVTIMEO, (char*)nNetTimeout, sizeof(int));
		//接收用户连接后发送的验证码，识别用户种类
		char recvBuf[100];
		memset(recvBuf, 0, sizeof(recvBuf));
		int irecv = recv(sockConn, recvBuf, sizeof(recvBuf), 0);

		if (irecv == 0)//超时，未收到用户发来的信息，断开该连接
		{
			//closesocket(sockConn);
		}
		else
		{
			printf("receive data:%s\n", recvBuf);
			if (recvBuf[0] == 'g')//发送来的信息，以g开头的为高级行情，后则为普通行情
			{
				WaitForSingleObject(sMutex, INFINITE);
				gjSockets.insert(gjSockets.end(), sockConn);
				printf("Accept client IP:[%s] as advanced user\n", inet_ntoa(addrClient.sin_addr));
				ReleaseMutex(sMutex);
			}			
		}
	}
	closesocket(sockSrv);
	WSACleanup();
	return 0;
}


