/******************************************************************************
                            
                Author  	: NGUYEN HUY THAI
                From 		: KT22_LEGEND
                Gmail     : thaibk.nh0601@gmail.com

*******************************************************************************/



/********************************************************************************
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
********************************************************************************/

#pragma comment(lib, "Ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <ws2tcpip.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

#define BUFF_SIZE 2048


int validate_IP(char *);
int validate_number(char *str);


int main(int argc, char ** argv) {


	//Step 0: validate input from arguments
	int port_number;
	char ipserverAddress[20];


	if (argc != 3) {
		printf("usage  : %s IPserverAddress PortNumber.\n", argv[0]);
		printf("Example: %s 127.0.0.1 5500\n", argv[0]);
		return 1;
	}
	else {
		//check ip serverAddress

		strcpy(ipserverAddress, argv[1]);
		//Note: after ipserverAddress pass on validate_IP function, it's will change value
		if (validate_IP(ipserverAddress) == 0) {
			printf("Failed!!!! Your ip serverAddress is invalid.Check against.\n");
			return 1;
		}

		//check port numner
		port_number = atoi(argv[2]);
		if (port_number <= 0 || port_number > 65353) {
			printf("Failed!!! Your port is invalid (please, give port in (0,65353] ).\n");
			return 1;
		}
	}
	printf("port_number: %d\t ip_server: %s\n", port_number, argv[1]);


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
	int tv = 10000;
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char *)(&tv), sizeof(int));

	//Step 3: Specify server address
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port_number);
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

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

	while(1) {
		memset(buff, '\0', BUFF_SIZE);
		printf("Send to server>> ");
		gets_s(buff, BUFF_SIZE);

		//Send buff to server
		ret = send(client, buff, strlen(buff), 0);
		if (ret == SOCKET_ERROR) {
			printf("Error,Cannot send message.\n");
			return 0;
		}
		if (strcmp(buff, "") == 0) break;

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
			printf("%s\n",buff);
		}
	}

	//Step 6: Close socket
	closesocket(client);

	//Step 7: Terminat winsock
	WSACleanup();

	_getch();
	return 0;
}



int validate_number(char *number) {
	while (*number != NULL) {
		if (!isdigit(*number)) return 0;
		number++;
	}
	return 1;
}

int validate_IP(char *ip) {
	int num, dots = 0;
	char *ptr;
	char *next_token;
	if (ip == NULL)
		return 0;
	ptr = strtok_s(ip, ".", &next_token);
	if (ptr == NULL)
		return 0;
	while (ptr) {
		//check number
		if (!validate_number(ptr))
			return 0;

		//validate token
		num = atoi(ptr);
		if (num >= 0 && num <= 255) {
			ptr = strtok_s(NULL, ".", &next_token);
			dots++;
		}
		else return 0;
	}
	if (dots != 4) return 0;
	return 1;
}