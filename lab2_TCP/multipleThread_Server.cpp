/******************************************************************************

			Author  	: NGUYEN HUY THAI
			From 		: KT22_LEGEND
			Gmail       : thaibk.nh0601@gmail.com

*******************************************************************************/

/* Multiple thread resolved the problem that block when waited or prcessed data take a long time */


/********************************************************************************
		Functions was used
			unsinged long _beginthreadex(
			void * security, // Tro toi cau truc xac dinh quyen truy cap
			unsinged stack_size, // kich thuoc khoi tao stack cho luong moi
			unsinged (__stdcall * start_address) (void*), //co tro tro toi ham thuc thi trong luon
			void * arglist, //con tro tro toi tham so truyen cho luong moi
			unsinged initflag, //co dieu khien tao luong
			unsinged * thraddr //tro toi gia tri dinh danh cua luong moi
)
********************************************************************************/


#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <process.h>

#include <ws2tcpip.h>
#include <winsock2.h>

#pragma comment (lib, "ws2_32.lib")

#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048
#define BUFF_CLIENT 1024

unsigned __stdcall echoThread(void *param);

int main(int argc, char ** argv) {
	//Step 0: validate input from arguments
	int port_number;
	if (argc != 2) {
		printf("usage  : %s PortNumber.\n", argv[0]);
		printf("Example: %s 5500\n", argv[0]);
		return 1;
	}
	else {

		//check port numner
		port_number = atoi(argv[1]);
		printf("port_number: %d\n", port_number);
		if (port_number <= 0 || port_number > 65353) {
			printf("Failed!!! Your port is invalid (please, give port in (0,65353] ).\n");
			return 1;
		}
	}


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
	serveAddr.sin_port = htons(port_number);
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

	//Step 5: Communication with client
	SOCKET connSocket;
	sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);

	while (1) {
		connSocket = accept(listenSock, (sockaddr *) &clientAddr, &clientAddrLen);
		printf("connSocket: %d\n", connSocket);
		_beginthreadex(0, 0, echoThread, (void *)connSocket, 0, 0);
	}

	//Step 6: Close listen socket
	closesocket(listenSock);

	WSACleanup();
	_getch();
	return 0;


}

unsigned __stdcall echoThread(void *param) {
	char buff[BUFF_SIZE];
	int ret;
	SOCKET connSock = (SOCKET)param;

	//receive message from client

	while (1) {
		ret = recv(connSock, buff, BUFF_SIZE, 0);
		if (ret == SOCKET_ERROR) {
			printf("Error : %d\n", WSAGetLastError());
			break;
		}
		else if (strlen(buff) > 0) {
			buff[ret] = '\0';
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
	return 0;
}

