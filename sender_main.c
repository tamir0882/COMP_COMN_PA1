#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib, "Ws2_32.lib")

#define FAILURE -1
#define SUCCESS 0




#define IP_ADDR_INDEX 1
#define CHANNEL_PORT_INDEX 2
#define FILE_INDEX 3
#define ARGC_COUNT 4

int main(int argc, char* argv[])
{
	if (argc != ARGC_COUNT)
	{
		fprintf(stderr, "ARGC_COUNT fault, should receive 3 argumets in addition to program.\n");
		exit(FAILURE);
	}

	int ret_val = 0;

	unsigned long ip_address = 0;
	char str_ip_address[17] = "";

	unsigned int channel_port = 0;


	ret_val = snprintf(str_ip_address, 17, "%s", argv[IP_ADDR_INDEX]);
	if (0 >= ret_val)
	{
		fprintf(stderr, "ARGC_COUNT fault, should receive 3 argumets in addition to program.\n");
		exit(FAILURE);
	}

	channel_port = (unsigned int)strtol(argv[CHANNEL_PORT_INDEX], NULL, 10);
	if (channel_port == 0)
	{
		printf("ERROR in main - strtol for has failed.\n");

	}



	// initialize windows networking
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");


	// create the socket that will listen for incoming TCP connections
	SOCKET s_sender = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in remote_addr;
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(REMOTE_HOST_IP);
	remote_addr.sin_port = htons(IN_PORT);

	sent = send(s, send_buf, MSG_SIZE, 0);
	printf(" --> %d Sent\n", sent);
	return 0;
}