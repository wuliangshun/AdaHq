#include "server.h"

extern int mdPt;//ת���˿�
vector<SOCKET> sockets;//��ͨ����ͻ���
vector<SOCKET> gjSockets;//�������ͻ���
HANDLE hMutex = CreateMutex(NULL, FALSE, NULL);
HANDLE sMutex = CreateMutex(NULL, FALSE, NULL);
queue<string> msgqueue;//��Ϣ����


DWORD WINAPI sendquotes(LPVOID lpParamter)
{
	//GetTickCount()���ú��������windows.h���õ�����ϵͳ���е�ʱ�� ��ȷ������
	//SYSTEMTIME lasttime, nowtime;//GetLocalTime(&lasttime);�ṹ�壬��������
	double lasttime, nowtime;
	lasttime = GetTickCount();

	while (1) {
		if (msgqueue.empty())
		{
			nowtime = GetTickCount();
			if (nowtime - lasttime > 3000)//ÿ�����뷢һ��heartbeat��Ϣ
			{
				WaitForSingleObject(hMutex, INFINITE);
				string s = "heartbeat;";
				msgqueue.push(s);
				ReleaseMutex(hMutex);
				lasttime = nowtime;
			}
			continue;
		}
		//sprintf(sendBuff, "Message %d is: ok", ncount++);    //��ncount��ֵ�����ַ���senBuff��
		//**********************  ������ʹ��sendto��������ͨ��    *************************// 
		WaitForSingleObject(hMutex, INFINITE);
		string s = msgqueue.front();
		msgqueue.pop();
		ReleaseMutex(hMutex);
		char *pPack = (char*)s.data();
		WaitForSingleObject(sMutex, INFINITE);
		vector<SOCKET>::iterator t;
		bool advanced = s[0] == 'o' || s[0] == 't' || s[0] == 'q';
		if (!advanced)//�Ǹ߼�����ŷ��͸���ͨ������
		{
			for (t = sockets.begin(); t < sockets.end(); t++)//��ͨ���鶩����
			{
				int iSend = send(*t, pPack, s.length(), 0);//��������  
				if (iSend == SOCKET_ERROR){
					printf("\nA client go offline ...\n\n");
					t = sockets.erase(t);
					if (t == sockets.end()) //Ҫ���Ƶ��������ܳ�����������
					{
						break;
					}
				}
			}
		}
		else //�κ����鶼���͸��߼�������
		{
			for (t = gjSockets.begin(); t < gjSockets.end(); t++)//�߼����鶩����
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

//�м������
DWORD WINAPI  middleServer(LPVOID lpParamter)
{
	WSADATA wsaData;

	char buf[] = "Server: hello, I am a server.....";

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("Failed to load Winsock");
		return -1;
	}

	//�������ڼ������׽���  
	SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(mdPt); //1024���ϵĶ˿ں�  
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

	CreateThread(NULL, 0, sendquotes, NULL, 0, NULL);//��������㲥�߳�	

	while (1)
	{
		//�ȴ��ͻ�������
		SOCKET sockConn = accept(sockSrv, (SOCKADDR *)&addrClient, &len);
		if (sockConn == SOCKET_ERROR){
			printf("Accept failed:%d", WSAGetLastError());
			break;
		}
		//Ĭ����ӵ���ͨ�û��б�
		WaitForSingleObject(sMutex, INFINITE);
		sockets.insert(sockets.end(), sockConn);
		printf("Accept client IP:[%s] as common user\n", inet_ntoa(addrClient.sin_addr));
		ReleaseMutex(sMutex);

		int nNetTimeout = 1000;//1�룬
		//���÷��ͳ�ʱ
		//setsockopt(socket��SOL_SOCKET, SO_SNDTIMEO��(char *)&nNetTimeout, sizeof(int));
		//���ý��ճ�ʱ
		//setsockopt(socket��SOL_SOCKET, SO_RCVTIMEO��(char *)&nNetTimeout, sizeof(int));
		setsockopt(sockConn, SOL_SOCKET, SO_RCVTIMEO, (char*)nNetTimeout, sizeof(int));
		//�����û����Ӻ��͵���֤�룬ʶ���û�����
		char recvBuf[100];
		memset(recvBuf, 0, sizeof(recvBuf));
		int irecv = recv(sockConn, recvBuf, sizeof(recvBuf), 0);

		if (irecv == 0)//��ʱ��δ�յ��û���������Ϣ���Ͽ�������
		{
			//closesocket(sockConn);
		}
		else
		{
			printf("receive data:%s\n", recvBuf);
			if (recvBuf[0] == 'g')//����������Ϣ����g��ͷ��Ϊ�߼����飬����Ϊ��ͨ����
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


