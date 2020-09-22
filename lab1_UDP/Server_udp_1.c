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
#define SERVER_IP "127.0.0.1"

int data_processed(char *, char *, char *);

int main(int argc, char **argv)
{
	//Step 0: validate input from arguments
	int port_number;
	char ipAddress[20];


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

	if (WSAStartup(wVersion, &wsaData) != 0) {
		printf("Version is not supported.\n");
		return 1;
	}

	//Step 2: Initiate socket
	SOCKET server;
	server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	//Step 3: bind address to socket
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_number);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	if (bind(server, (sockaddr*)&addr, sizeof(addr))) {
		printf("Error! Cannot bind this address.\n");
		_getch();
		return 0;
	}

	printf("Server started!!!\n");

	//Step 4: Communicate with client
	sockaddr_in clientAddr;
	char buff[BUFF_SIZE], buff_number[BUFF_SIZE], buff_alphabet[BUFF_SIZE];
	int ret, result, clientAddrLen = sizeof(clientAddr);
	/*when contact with client, information of client will save in clientAddr
	about: + ip address, port number, information after server process
	*/

	while (1) {
		//Receive message
		ret = recvfrom(server, buff, BUFF_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrLen);
		if (ret == SOCKET_ERROR)
			printf("Error: %d\n", WSAGetLastError());
		else if (strlen(buff) > 0) {
			buff[ret] = 0;
			printf("Receive from client [%s:%d] %s.\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff);
			/*
			Code
			0 : data's client contains special character
			1 : data's client only contains alphabet
			2 : data's client only contains number
			3 : data's client contains number and alphabet

			*/
			result = data_processed(buff, buff_number, buff_alphabet);
			buff[0] = 0;


			//Send result data processed to client
			switch (result) {
			case 0:
				//send error message
				printf("Case 0\n");
				strcpy(buff, "Error: Input string contains special character");
				ret = sendto(server, buff, strlen(buff), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
				if (ret == SOCKET_ERROR) {
					printf("Error!Send data with case 0.\n");
					printf("Error: %d", WSAGetLastError());
				}
				printf("Size of data send to client: %d\n", ret);
				break;

			case 1:
				//send buff_alphanet
				printf("Case 1\n");
				ret = sendto(server, buff_alphabet, strlen(buff_alphabet), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
				if (ret == SOCKET_ERROR) {
					printf("Error!Send data with case 1.\n");
					printf("Error: %d", WSAGetLastError());
				}
				printf("Size of data send to client: %d\n", ret);
				break;

			case 2:
				//send buff_number
				printf("Case 2\n");
				ret = sendto(server, buff_number, strlen(buff_number), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
				if (ret == SOCKET_ERROR) {
					printf("Error!Send data with case 2.\n");
					printf("Error: %d", WSAGetLastError());
				}
				printf("Size of data send to client: %d\n", ret);
				break;

			case 3:
				//send buff_number and buff_alphabet
				printf("Case 3\n");
				buff[0] = 0;
				strcat(buff, buff_number);
				strcat(buff, "\n");
				strcat(buff, buff_alphabet);
				ret = sendto(server, buff, strlen(buff), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
				if (ret == SOCKET_ERROR) {
					printf("Error!Send data with case 3.\n");
					printf("Error: %d", WSAGetLastError());
				}
				printf("Size of data send to client: %d\n", ret);
				break;

			default:
					printf("Error in switch function\n");
			}
			printf("Response data processed client success!!!\n");

		}
	}//end while

	 //Step 5: Close socket
	closesocket(server);

	//Step 6: Terminate winsock
	WSACleanup();

	return 0;

}

int data_processed(char *buff, char *buff_number, char *buff_alphabet) {
	int i, j;
	i = j = 0;
	int length = strlen(buff);

	for (int k = 0; k < length; k++) {
		if (isalpha(buff[k]) != 0) {
			buff_alphabet[i] = buff[k];
			i++;
		}
		else if (isdigit(buff[k]) != 0) {
			buff_number[j] = buff[k];
			j++;
		}
		else return 0;
	}
	buff_alphabet[i] = '\0';
	buff_number[j] = '\0';
	if (strlen(buff_number) == 0) return 1;
	else if (strlen(buff_alphabet) == 0) return 2;
	else return 3;

}




