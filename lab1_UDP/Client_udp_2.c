/******************************************************************************
                            
                Author  	: NGUYEN HUY THAI
                From 		: KT22_LEGEND
                Gmail     : thaibk.nh0601@gmail.com

*******************************************************************************/

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

int main(int argc, char **argv)
{
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
		if (validate_IP(ipserverAddress) == 0) {
			printf("Failed!!!! Your ip serverAddress is invalid.Check against.\n");
			return 1;
		}

		//check port numner
		port_number = atoi(argv[2]);
		printf("port_number: %d\n", port_number);
		if (port_number <= 0 || port_number > 65353) {
			printf("Failed!!! Your port is invalid (please, give port in (0,65353] ).\n");
			return 1;
		}
	}



	//Step 1: Initiate winsock
	WSADATA wsaData;
	WORD wVersion = MAKEWORD(2, 2);

	if (WSAStartup(wVersion, &wsaData) != 0) {
		printf("Version is not supported.\n");
		return 1;
	}

	printf("Client started!\n");




	//Step 2: Initiate socket
	SOCKET client;
	client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//set time-out when send data to server
	int tv = 10000;//time-out 10000ms
	setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, (const char *)(&tv), sizeof(int));
	

	//Step 3: bind serverAddress to socket
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port_number);
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);


	//Step 4: Communicate with server
	char buff[BUFF_SIZE];
	int ret, serverAddrLen = sizeof(serverAddr);
	/*when contact with client, information of client will save in serverserverAddr
	about: + ip serverAddress, port number, information after server process
	*/

	do {
		ZeroMemory(buff, BUFF_SIZE);
		fflush(stdin);
		//Send data to server
		printf("Message to server >> ");
		gets_s(buff, BUFF_SIZE);
		ret = sendto(client, buff, strlen(buff), 0, (sockaddr *)&serverAddr, serverAddrLen);
		if (ret == SOCKET_ERROR) {
			printf("Send data to server with error, code: %d\n", WSAGetLastError());
		}
		else {
			//Receive data from server
			buff[0] = 0;
			ret = recvfrom(client, buff, BUFF_SIZE, 0, (sockaddr*)&serverAddr, &serverAddrLen);
			if (ret == SOCKET_ERROR) {		
				printf("Error! Cannot receive message.");
				printf("Code error: %d\n", WSAGetLastError());
			}
			else if (strlen(buff) > 0) {
				buff[ret] = 0;
				printf("Receive from server: \n");
				printf("%s\n", buff);
			}

		}

		//Convert to Uppercase letter

	} while (strcmp(buff, "bye") != 0);//end while

									   //Step 5: Close socket
	closesocket(client);

	//Step 6: Terminate winsock
	WSACleanup();

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
