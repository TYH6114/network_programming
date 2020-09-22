#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
/*
Processed in client
+ connect(
SOCKET s; //socket client using commnication with server
const struct sockaddr * name, //address of server
int namelen //sizeof(name)
)
+ value return: 0 (success), SOCKET_ERROR (failed)

+ send(
SOCKET s, \\socket was used
const char *buff, \\buffer contains data
int len, \\ sizeof(buff)
int flags \\ flags control (0)
)
*/

#include <stdio.h>
#include <conio.h>
#include <string.h>

#include <WinSock2.h>
#include <WS2tcpip.h>

#define SERVER_PORT 5500
#define SERVER_ADDR "127.0.0.1"
#define BUFF_SIZE 2048

int main(int argc, char ** argv) {
	//Step 1: Initiate winsock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);
	if (WSAStartup(wVersion, &wsaData)) {
		printf("Version is not supported.\n");
		return 0;
	}

	//Step 2: Construct socket
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//Set time-out for receiving: 1000ms
	int tv = 1000;
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char *)(&tv), sizeof(int));

	//Step 3: Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

	//Step 4: Request to connect server
	if (connect(client, (sockaddr *)&serverAddr, sizeof(serverAddr))) {
		printf("Error ! Cannot connect server. Error code: %d\n", WSAGetLastError());
		_getch();
		return 0;
	}

	printf("Connected server!!!.\n");

	//Step 5: Communicate with client

	char buff[BUFF_SIZE];
	int ret;

	do {
		printf("Send to server>> ");
		gets_s(buff, BUFF_SIZE);

		//Send buff to server
		ret = send(client, buff, strlen(buff), 0);
		if (ret == SOCKET_ERROR) {
			printf("Error,Cannot send message.\n");
			return 0;
		}

		//Receive echo message
		ret = recv(client, buff, BUFF_SIZE, 0);
		if (ret == SOCKET_ERROR) {
			if (WSAGetLastError() == WSAETIMEDOUT)
				printf("Time-out");
			else
				printf("Error, Cannot receive message.");
		}
		else if (strlen(buff) > 0) {
			buff[ret] = '\0';
			printf("Receive from server [%s:%d] %s.\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port), buff);
		}

		_strupr_s(buff);

	} while (strcmp(buff, "BYE"));

	//Step 6: Close socket
	closesocket(client);

	//Step 7: Terminat winsock
	WSACleanup();

	_getch();
	return 0;
}