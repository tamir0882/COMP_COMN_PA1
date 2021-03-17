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
#define MSG_SIZE 15
#define CHUNK_SIZE 15

int main(int argc, char* argv[])
{
	if (argc != ARGC_COUNT)
	{
		fprintf(stderr, "ARGC_COUNT fault, should receive 5 argumets in addition to program.\n");
		exit(FAILURE);
	}

	int exit_code = SUCCESS;
	int ret_val = 0;
	char recv_ip_address[17] = "";
	unsigned int error_prob = 0;
	unsigned int rand_seed = 0;
	unsigned int channel_port = 0;
	unsigned int receiver_port = 0;
	unsigned int error_num = 0;
	unsigned int prob_den = 0;
	unsigned int i = 0;
	char* received_buffer[15] = "";
	
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
	
	// Create a socket to the channel to receive the data

	SOCKET s_channel = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (SOCKET_ERROR == s_channel)
	{
		printf("ERROR - failed to create s_sender socket - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	struct sockaddr_in channel_addr;
	channel_addr.sin_family = AF_INET;
	channel_addr.sin_addr.s_addr = INADDR_ANY;
	channel_addr.sin_port = htons(channel_port);

	SOCKADDR_IN channel_addr = { .sin_addr = 0,.sin_family = 0,.sin_port = 0 };

	ret_val = inet_pton(AF_INET, sender_ip_address, &sender_ip_address); ////// GET IP FROM SENDER
	if (1 != ret_val)
	{
		printf("ERROR - inet_pton failed - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}


	// Create a socket to the receiver to send the operated data

	SOCKET s_receiver = socket(AF_INET, SOCK_DGRAM, 0);

	if (SOCKET_ERROR == s_receiver)
	{
		printf("ERROR - failed to create s_sender socket - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	struct sockaddr_in recv_addr;
	recv_addr.sin_family = AF_INET;
	recv_addr.sin_addr.s_addr = inet_addr(recv_ip_address);
	recv_addr.sin_port = htons(channel_port);

	SOCKADDR_IN recv_addr = { .sin_addr = 0,.sin_family = 0,.sin_port = 0 };

	ret_val = inet_pton(AF_INET, recv_ip_address, &recv_ip_address);
	if (1 != ret_val)
	{
		printf("ERROR - inet_pton failed - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}


	ret_val = bind(s_channel, (SOCKADDR*)&channel_addr, sizeof(channel_addr));
	if (ret_val != 0)
	{
		printf("bind failed with error %d\n", WSAGetLastError());
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	received_buffer = (char*)malloc(sizeof(char) * CHUNK_SIZE);
	if (NULL == received_buffer)
	{
		printf("memory allocation failed for recv_buffer.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	ret_val = listen(s_channel, SOMAXCONN);

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
	

	prob_den = pow(BASE, POWER);
	if (error_num >= prob_den || error_num < 0)
	{
		error_num %= prob_den;
	}

	srand(rand_seed);
	
	for (i = 0; i < MSG_SIZE; i++) 
	{
		if ((rand() <= error_num) && (rand() % 2 == 0))
		{
		 
			received_buffer[i] ^= 1UL << i;
		}
	}

CleanUp:


	if (INVALID_SOCKET != s_channel)
	{
		if (SOCKET_ERROR == closesocket(s_channel))
		{
			printf("channel_main: Failed to close listen_socket, error %ld.\n", WSAGetLastError());
		}
	}


	if (SOCKET_ERROR == WSACleanup())
	{
		printf("channel_main: Failed to close Winsocket, error %ld.\n", WSAGetLastError());
	}



	return exit_code;
}

