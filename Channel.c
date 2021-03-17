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

#define CHANNEL_PORT_INDEX 1
#define RECV_IP_INDEX 2
#define RECV_PORT_INDEX 3
#define ERROR_PROB_INDEX 4
#define SEED_INDEX 5
#define ARGC_COUNT 6
#define BASE 2
#define POWER 16

int main(int argc, char* argv[])
{
	if (argc != ARGC_COUNT)
	{
		fprintf(stderr, "ARGC_COUNT fault, should receive 5 argumets in addition to program.\n");
		exit(FAILURE);
	}

	int ret_val = 0;
	char recv_ip_address[17] = "";
	unsigned int error_prob = 0;
	unsigned int rand_seed = 0;
	unsigned int channel_port = 0;
	unsigned int receiver_port = 0;
	unsigned int error_num = 0;
	int i = 0;
	
	
	ret_val = snprintf(recv_ip_address, 17, "%s", argv[RECV_IP_INDEX]);
	if (0 >= ret_val)
	{
		fprintf(stderr, "ARGC_COUNT fault, should receive 3 argumets in addition to program.\n");
		exit(FAILURE);
	}

	channel_port = (unsigned int)strtol(argv[CHANNEL_PORT_INDEX], NULL, 10);
	if (channel_port == 0)
	{
		printf("ERROR in main - strtol for has failed.\n");
		exit(FAILURE);
	}

	receiver_port = (unsigned int)strtol(argv[RECV_PORT_INDEX], NULL, 10);
	if (receiver_port == 0)
	{
		printf("ERROR in main - strtol for has failed.\n");
		exit(FAILURE);
	}

	error_num = (unsigned int)strtol(argv[ERROR_PROB_INDEX], NULL, 10);
	if (error_num == 0)
	{
		printf("ERROR in main - strtol for has failed.\n");
		exit(FAILURE);
	}

	rand_seed = (unsigned int)strtol(argv[SEED_INDEX], NULL, 10);
	if (rand_seed == 0)
	{
		printf("ERROR in main - strtol for has failed.\n");
		exit(FAILURE);
	}

	error_prob = error_num / pow(BASE, POWER);

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
		exit(FAILURE);
	}
	
	SOCKADDR_IN server_addr;

	SOCKET s_channel = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in remote_addr;
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = INADDR_ANY;
	remote_addr.sin_port = htons(channel_port);

	SOCKET s_receiver = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	struct sockaddr_in remote_addr;
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = inet_addr(recv_ip_address);
	remote_addr.sin_port = htons(channel_port);

	int status = bind(s_channel, (SOCKADDR*)&server_addr, sizeof(server_addr));
	status = listen(s_channel, SOMAXCONN);

	/* CHANNEL RECEIVES MESSAGES FROM SENDER - NEED TO BE MODIFIED
	while (1)
	{
		sscanf
			//here need to change for udp communication
			received = recvfrom(s, recieved_buffer, MSG_SIZE, 0);
		if (received)
			printf("%s", recieved_buffer);
	}
	
	*/
	
	for (i = 0; i < MSG_SIZE; i++)
	{
		int is_changed = srand(rand_seed) / (RAND_MAX + 1) * ;
	}
}
