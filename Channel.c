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

#include "communication.h"

#define CHANNEL_PORT_INDEX 1
#define RECV_IP_INDEX 2
#define RECV_PORT_INDEX 3
#define ERROR_PROB_INDEX 4
#define SEED_INDEX 5
#define ARGC_COUNT 6
#define BASE 2
#define POWER 16
#define MSG_SIZE 15
#define CHUNK_SIZE 1500

int main(int argc, char* argv[])
{
	if (argc != ARGC_COUNT)
	{
		fprintf(stderr, "ARGC_COUNT fault, should receive 5 argumets in addition to program.\n");
		exit(FAILURE);
	}

	int exit_code = SUCCESS;
	int ret_val = 0;
	int bytes_recv = 0;
	int flipped_count = 0;
	unsigned int error_prob = 0;
	unsigned int rand_seed = 0;
	unsigned int channel_port = 0;
	unsigned int receiver_port = 0;
	unsigned int error_num = 0;
	unsigned int prob_den = 0;
	unsigned int rand_num = 0;
	char str_recv_ip_address[17] = "";
	char str_sender_ip_address[17] = "";
	char* recv_buffer = NULL;
	unsigned long recv_ip_address = 0;
	
	
	SOCKET s_channel = INVALID_SOCKET;
	SOCKET s_recv = INVALID_SOCKET;
	SOCKADDR_IN sender_addr = { .sin_addr = 0,.sin_family = 0,.sin_port = 0 };


	recv_buffer = (char*)calloc(CHUNK_SIZE, sizeof(char));
	if (NULL == recv_buffer)
	{
		printf("ERROR in channel main - memory allocation failed for recv_buffer.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	ret_val = snprintf(str_recv_ip_address, 17, "%s", argv[RECV_IP_INDEX]);
	if (0 >= ret_val)
	{
		fprintf(stderr, "ERROR in channel main - ARGC_COUNT fault, should receive 3 argumets in addition to program.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	channel_port = (unsigned int)strtol(argv[CHANNEL_PORT_INDEX], NULL, 10);
	if (channel_port == 0)
	{
		printf("ERROR in channel main - strtol has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	receiver_port = (unsigned int)strtol(argv[RECV_PORT_INDEX], NULL, 10);
	if (receiver_port == 0)
	{
		printf("ERROR in channel main - strtol has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	error_num = (unsigned int)strtol(argv[ERROR_PROB_INDEX], NULL, 10);
	if (error_num == 0)
	{
		printf("ERROR in channel main - strtol has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	rand_seed = (unsigned int)strtol(argv[SEED_INDEX], NULL, 10);
	if (rand_seed == 0)
	{
		printf("ERROR in channel main - strtol has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
	{
		printf("ERROR in channel main - Error at WSAStartup()\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	// Create a socket to the channel to receive the data
	
	s_channel = socket(AF_INET, SOCK_DGRAM, 0);
	
	if (SOCKET_ERROR == s_channel)
	{
		printf("ERROR in channel main - failed to create s_channel socket.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	SOCKADDR_IN channel_addr;
	channel_addr.sin_family = AF_INET;
	channel_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	channel_addr.sin_port = htons(channel_port);

	// Create a socket to the receiver to send the operated data

	s_recv = socket(AF_INET, SOCK_DGRAM, 0);

	if (SOCKET_ERROR == s_recv)
	{
		printf("ERROR in channel main - failed to create s_recv socket.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	ret_val = inet_pton(AF_INET, str_recv_ip_address, &recv_ip_address);
	if (1 != ret_val)
	{
		printf("ERROR in channel main - inet_pton has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	SOCKADDR_IN recv_addr;
	recv_addr.sin_family = AF_INET;
	recv_addr.sin_addr.s_addr = recv_ip_address;
	recv_addr.sin_port = htons(channel_port);

	ret_val = bind(s_channel, (SOCKADDR*)&channel_addr, sizeof(channel_addr));
	if (ret_val != 0)
	{
		printf("ERROR in channel main - bind failed with error %d\n", WSAGetLastError());
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	srand(rand_seed);

FromSendertoRecv: ////UNREFERENCED_LABEL - if not being used, delete.

	while (1)
	{
		bytes_recv = recv_data(recv_buffer, CHUNK_SIZE, s_channel, &sender_addr);
		if (FAILURE == bytes_recv)
		{
			printf("ERROR in channel main - recv_data failed.\n");
			exit_code = FAILURE;
			goto CleanUp;
		}

		for (int i = 0; i < bytes_recv * 8; i++)
		{
			rand_num = rand();
			if (rand() % 2)
			{
				rand_num = rand_num*2 + 1;
			}
			else
			{
				rand_num *= 2;
			}
			if (rand_num < error_num)
			{
				recv_buffer[i] ^= 1UL << i;
				flipped_count += 1;
			}
		}

		ret_val = send_data(recv_buffer, bytes_recv, s_recv, recv_addr);
		if (FAILURE == ret_val)
		{
			printf("ERROR in channel main - send_string failed.\n");
			exit_code = FAILURE;
			goto CleanUp;
		}
	}
		
	if(NULL == inet_ntop(AF_INET, &sender_addr.sin_addr.s_addr, str_sender_ip_address, sizeof(str_sender_ip_address)))
	{
		printf("ERROR in channel main - couldn't convert IP address to string.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	fprintf(stderr, "sender: %s\nreceiver: %s\n%d bytes, flipped %d bits", str_sender_ip_address, str_recv_ip_address, sizeof(recv_buffer), flipped_count);


FromRecvtoSender: ////UNREFERENCED_LABEL - if not being used, delete.

	while (1)
	{
		bytes_recv = recv_data(recv_buffer, CHUNK_SIZE, s_recv, &recv_addr);
		if (FAILURE == bytes_recv)
		{
			printf("ERROR in channel main - recv_data failed.\n");
			exit_code = FAILURE;
			goto CleanUp;
		}

		ret_val = send_data(recv_buffer, bytes_recv, s_channel, sender_addr);
		if (FAILURE == ret_val)
		{
			printf("ERROR in channel main - send_string failed.\n");
			exit_code = FAILURE;
			goto CleanUp;
		}
		closesocket(s_channel);
	}


CleanUp:

	if (INVALID_SOCKET != s_channel)
	{
		if (SOCKET_ERROR == closesocket(s_channel))
		{
			printf("ERROR in channel main - Failed to close channel_socket, error %ld.\n", WSAGetLastError());
		}
	}
	
	if (INVALID_SOCKET != s_recv)
	{
		if (SOCKET_ERROR == closesocket(s_recv))
		{
			printf("ERROR in channel main - Failed to close receive_socket, error %ld.\n", WSAGetLastError());
		}
	}


	if (SOCKET_ERROR == WSACleanup())
	{
		printf("ERROR in channel main - Failed to close Winsocket, error %ld.\n", WSAGetLastError());
	}

	if (NULL != recv_buffer)
	{
		free(recv_buffer);
	}

	return exit_code;
}
