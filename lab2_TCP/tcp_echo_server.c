/******************************************************************************
                            
                Author  	: NGUYEN HUY THAI
                From 		: KT22_LEGEND
                Gmail     : thaibk.nh0601@gmail.com

*******************************************************************************/



#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <conio.h>
#include <stdio.h>

#include <winsock2.h>
#include <WS2tcpip.h>

#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.01"
#define BUFF_SIZE 2048

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char **argv) {
	//Step 1: Initiate winsock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Version is not supported\n");
		return 0;
	}

	//Step 2: Construct socket
	SOCKET listenSock;
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Step 3: Bind address to socket
	sockaddr_in serveAddr;
	serveAddr.sin_family = AF_INET;
	serveAddr.sin_port = htons(SERVER_PORT);
	serveAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	if (bind(listenSock, (sockaddr*)&serveAddr, sizeof(serveAddr)))
	{
		printf("Error! Cannot bind this address.");
		_getch();
		return 0;
	}

	//Step 4: Listen request from client
	if (listen(listenSock, 10)) {
		printf("Error! Cannot listen.");
		_getch();
		return 0;
	}

	printf("Server started!!!.\n");

	//Step 5: Communicate with client
	sockaddr_in clientAddr;
	char buff[BUFF_SIZE];
	int ret, clientAddrLen = sizeof(clientAddr);

	while (1) {
		SOCKET connSock;//socket was returned for communication with client

						//accept request from queue
		connSock = accept(listenSock, (sockaddr *)&clientAddr, &clientAddrLen);
		if (connSock == SOCKET_ERROR) {
			printf("Accept with error\n.");
			return 0;
		}

		//receive message from client

		while (1) {
			ret = recv(connSock, buff, BUFF_SIZE, 0);
			if (ret == SOCKET_ERROR) {
				printf("Error : %d\n", WSAGetLastError());
				break;
			}
			else if (strlen(buff) > 0) {
				buff[ret] = '\0';
				printf("Receive from client [%s:%d] %s\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff);

				//Echo to client
				ret = send(connSock, buff, strlen(buff), 0);
				if (ret == SOCKET_ERROR)
					printf("Error : %d\n", WSAGetLastError());
				else {
					_strupr_s(buff);
					if (strcmp(buff, "BYE") == 0) break;
				}
			}
		}
		printf("Client quit\n");

		closesocket(connSock);
	} //end accepting

	  //Step 5: Close socket
	closesocket(listenSock);

	//Step 6: Terminate winsock
	WSACleanup();

	return 0;


}


