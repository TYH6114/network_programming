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
#define SERVER_IP "127.0.0.1"


int validate_ipv4(char *);
int validate_ipv6(char *);
int validate_number(char *str);
int check_hexa(char);


int main(int argc, char **argv)
{
	//Step 0: validate input from arguments
	int port_number;
	char  ip_client[BUFF_SIZE];


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
	char buff[BUFF_SIZE];
	int ret, clientAddrLen = sizeof(clientAddr);
	/*when contact with client, information of client will save in clientAddr
	about: + ip address, port number, information after server process
	*/

	//Initiate struct containt information after resolution ipv4
	struct hostent *remoteHost;
	char **pAlias;
	struct in_addr ip_addr = { 0 };
	DWORD dwError;
	int i, temp;

	//Initiate struct containt information after resolution ipv6
	int ipv6_result;
	IN6_ADDR addr6;
	char domain_client[BUFF_SIZE];
	struct in_addr ip_Resolution;


	while (1) {
		//Receive message
		ret = recvfrom(server, buff, BUFF_SIZE, 0, (sockaddr*)&clientAddr, &clientAddrLen);
		if (ret == SOCKET_ERROR)
			printf("Error: %d\n", WSAGetLastError());
		else if (strlen(buff) > 0) {
			buff[ret] = '\0';
			printf("Receive from client [%s:%d] %s.\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), buff);


			//data processed

			/*
			Case1: Receive a ip address from client
			*/

			strcpy(ip_client, buff);
			strcpy(domain_client, buff);
			domain_client[ret] = '\0';
			ip_client[ret] = '\0';

			// if ip with the type ip address
			// Note: after using validate_ipv4 buff will change value
			if (validate_ipv6(buff) == 1 || validate_ipv4(buff) == 1) {
				printf("ip_client is %s\n", ip_client);
				/*
				hostent * WSAAPI gethostbyaddr(
				const char *addr,
				int len,//4: ipv4 6: ipv6
				int type //AF_INET : IPV4 AF_INET6: ipv6
				)

				*/
				memset(buff, '\0', BUFF_SIZE);//reset buff

				if (validate_ipv6(ip_client) == 1) { //if ip is ipv6
					printf("\tipv6: %s\n", ip_client);
					ipv6_result = inet_pton(AF_INET6, ip_client, &addr6);
					if (ipv6_result == 0) {
						printf("ipv6 address entered must a legal address ");
						strcpy(buff, "IP Address is invalid");
						ret = sendto(server, buff, strlen(buff), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
						if (ret == SOCKET_ERROR) {
							printf("Error!Send data to client.\n");
							printf("Error: %d", WSAGetLastError());
						}

					}
					else {
						remoteHost = gethostbyaddr((char *)&addr6, 16, AF_INET6);
					}

				}
				else {

					ip_addr.s_addr = inet_addr(ip_client);
					if (ip_addr.s_addr == INADDR_NONE) {
						printf("ipv4 address entered must a legal address ");
						strcpy(buff, "IP Address is invalid");
						ret = sendto(server, buff, strlen(buff), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
						if (ret == SOCKET_ERROR) {
							printf("Error!Send data to client.\n");
							printf("Error : %d", WSAGetLastError());
						}
					}
					else
						remoteHost = gethostbyaddr((char *)&ip_addr, 4, AF_INET);//resolution ipv4 address
				}

				if (remoteHost == NULL) {//if resolution not response any information about ipv4 address
					printf("Resolution %s,Not found any information.\n",ip_client);
					strcpy(buff, "IP Address is invalid");
					ret = sendto(server, buff, strlen(buff), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
					if (ret == SOCKET_ERROR) {
						printf("Error!Send data to client.\n");
						printf("Error: %d", WSAGetLastError());
					}

				}
				else {
					strcat(buff, "Official IP: ");
					strcat(buff, remoteHost->h_name);
					strcat(buff, "\n");
					temp = 0;
					for (pAlias = remoteHost->h_aliases; *pAlias != 0; pAlias++) {
						if (temp == 0) {
							strcat(buff, "Alias IP:");
							temp++;
						}
						strcat(buff, "\n");
						strcat(buff, *pAlias);
					}
					ret = sendto(server, buff, strlen(buff), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
					if (ret == SOCKET_ERROR) {
						printf("Error!Send data to client.\n");
						printf("Error: %d", WSAGetLastError());
					}
				}

			}
			else { // if receive domain from client
					remoteHost = gethostbyname(domain_client);
					memset(buff, '\0', BUFF_SIZE);

					//Case 1: Not found information of ip
					if (remoteHost == NULL) {
						dwError = WSAGetLastError();
						printf("Can't find any information. Error code: %d\n", dwError);
						strcpy(buff, "Not found information");
						ret = sendto(server, buff, strlen(buff), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
						if (ret == SOCKET_ERROR) {
							printf("Error!Send data with case 3.\n");
							printf("Error: %d", WSAGetLastError());
						}
					}
					else {

						i = 0;
						strcat(buff, "Official IP: ");
						while (remoteHost->h_addr_list[i] != 0) {
							if (i == 1) strcat(buff, "\nAlias IP:");
							ip_Resolution.s_addr = *(u_long *)remoteHost->h_addr_list[i];
							if (i == 0) strcat(buff, inet_ntoa(ip_Resolution));
							else {
								strcat(buff, "\n");
								strcat(buff, inet_ntoa(ip_Resolution));
							}

							i++;
						}

						ret = sendto(server, buff, strlen(buff), 0, (sockaddr*)&clientAddr, sizeof(clientAddr));
						if (ret == SOCKET_ERROR) {
							printf("Error!Send data to client.\n");
							printf("Error: %d", WSAGetLastError());
						}
					}
				}
			}
		
	}//end while

	 //Step 5: Close socket
	closesocket(server);

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

//validate ipv4
int validate_ipv4(char *ip) {
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

//validate ipv6
int validate_ipv6(char *ip_string) {
	int temp = 0;
	int temp2 = 0;
	while ((*ip_string) != '\0') {
		if (*ip_string == ':') {
			if (*(ip_string + 1) == ':') {
				temp2++;
				ip_string += 2;
			}
			else {
				temp++;
				ip_string++;
			}
		}
		else if (check_hexa(*ip_string) == 1) {
			ip_string++;
		}
		else return 0;
	}
	if ((temp == 7) || (temp < 7 && temp2 >0)) return 1;
	else return 0;

}

int check_hexa(char c) {
	if (('0' <= c && c <= '9') || ('A' <= c && c <= 'F')) return 1;
	else return 0;
}



