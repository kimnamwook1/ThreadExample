#define _WINSOCK_DEPRECATED_NO_WARNINGS 
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <WinSock2.h>
#include <conio.h>
#include <string>
#include <process.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")

unsigned WINAPI RecvThread(void* Parameters)
{
	SOCKET ServerSocket = *(SOCKET*)Parameters;
	char Buffer[1024] = { 0, };
	while (true)
	{
		memset(Buffer, 0, sizeof(Buffer));
		int RecvLength = recv(ServerSocket, Buffer, sizeof(Buffer), 0);
		if (RecvLength == 0)
		{
			//연결종료
			std::cout << "Disconnected Client" << std::endl;
			break;
		}
		else if (RecvLength < 0)
		{
			//error
			std::cout << "Disconnected Client By Error" << GetLastError() << std::endl;
			break;
		}
		cout << "Server Sended : " << Buffer << endl;
	}
	return 0;
}

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ServerAddrIn;
	memset(&ServerAddrIn, 0, sizeof(SOCKADDR_IN));
	//char* p = (char*)&ServerAddrIn;
	ServerAddrIn.sin_family = AF_INET;
	ServerAddrIn.sin_port = htons(1234);
	ServerAddrIn.sin_addr.s_addr = inet_addr("172.16.5.216");

	connect(ServerSocket, (SOCKADDR*)&ServerAddrIn, sizeof(SOCKADDR_IN));

	cout << "Please Type your Message." << endl;

	HANDLE RecvThreadHandle = (HANDLE)_beginthreadex(nullptr, 0, RecvThread, (void*)&ServerSocket, 0, nullptr);

	bool bRunning = true;
	char SendMessage[1024] = { 0, };
	while (bRunning)
	{		
		cin.getline(SendMessage, 1024-1);

		int SendLength = send(ServerSocket, SendMessage, strlen(SendMessage), 0);
	}


	closesocket(ServerSocket);

	WSACleanup();

	return 0;
}