#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

#include "communication.h"
#pragma comment(lib, "Ws2_32.lib")

#define PORT_INDEX 1
#define FILE_INDEX 2
#define ARGC_COUNT 3

#define FAILURE -1 
#define SUCCESS 0

#define CHUNK_SIZE 1500

int main(int argc, char* argv[])
{
	
	if (argc != ARGC_COUNT)
	{
		printf("ARGC_COUNT fault, should receive %d argumets with program execution.\n", (ARGC_COUNT - 1));
		exit(FAILURE);
	}

	int ret_val = 0;
	int exit_code = SUCCESS;

	int total_byte_count = 0;
	char* file_data = NULL;

	int recv_bytes = 0;
	char* recv_buffer = NULL;

	int fix_count = 0;

	unsigned long channel_ip_address = 0;
	char str_channel_ip_address[17] = "";

	unsigned int port_num = 0;

	char file_name[_MAX_PATH] = "";
	FILE* p_file = NULL;

	SOCKADDR_IN sender_addr = { .sin_addr = 0, .sin_family = 0, .sin_port = 0 };

	SOCKADDR_IN recv_addr = { .sin_addr = 0, .sin_family = 0, .sin_port = 0 };
	SOCKET s_recv = INVALID_SOCKET;



	ret_val = snprintf(file_name, _MAX_PATH, "%s", argv[FILE_INDEX]);
	if (0 >= ret_val)
	{
		printf("ERROR - snprintf for file_name - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	port_num = (unsigned int)strtol(argv[PORT_INDEX], NULL, 10);
	if (port_num == 0)
	{
		printf("ERROR - strtol for channel_port - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	
	// initialize windows networking
	WSADATA wsaData;
	ret_val = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret_val != NO_ERROR)
	{
		printf("Error at WSAStartup()\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
		


	s_recv = socket(AF_INET, SOCK_DGRAM, 0);
	if (SOCKET_ERROR == s_recv)
	{
		printf("ERROR - socket creation for recv_socket - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	/*
	ret_val = inet_pton(AF_INET, "127.0.0.1", &channel_ip_address);
	if (1 != ret_val)
	{
		printf("ERROR - inet_pton failed - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	*/


	recv_addr.sin_family = AF_INET;
	//recv_addr.sin_addr.s_addr = channel_ip_address;
	recv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	recv_addr.sin_port = htons(port_num);
	

	ret_val = bind(s_recv, (SOCKADDR*)&recv_addr, sizeof(recv_addr));
	if (ret_val != 0) 
	{
		printf("bind failed with error %d\n", WSAGetLastError());
		exit_code = FAILURE;
		goto CleanUp;
	}
	

	ret_val = fopen_s(&p_file, file_name, "wb");
	if (0 != ret_val || NULL == p_file)
	{
		printf("ERROR - failed to open file %s.\n", file_name);
		exit_code = FAILURE;
		goto CleanUp;
	}

	
	recv_buffer = (char*)calloc(CHUNK_SIZE, sizeof(char));
	if (NULL == recv_buffer)
	{
		printf("memory allocation failed for recv_buffer.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	
	recv_bytes = recv_data(recv_buffer, CHUNK_SIZE, s_recv, &sender_addr);
	if (FAILURE == recv_bytes)
	{
		printf("ERROR - recv_data failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	fix_count = decode_str(recv_buffer, &recv_bytes);

	printf("sender sent: %s\n", recv_buffer);

	ret_val = fwrite(recv_buffer, sizeof(char), recv_bytes, p_file);
	if (ret_val < recv_bytes)
	{
		printf("ERROR - fwrite failure\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

CleanUp:

	if (NULL != recv_buffer)
	{
		free(recv_buffer);
	}

	if (NULL != p_file)
	{
		if (0 != fclose(p_file))
		{
			printf("could't close file.\n");
		}
	}


	if (INVALID_SOCKET != s_recv)
	{
		if (SOCKET_ERROR == closesocket(s_recv))
		{
			printf("server_main: Failed to close listen_socket, error %ld.\n", WSAGetLastError());
		}
	}


	if (SOCKET_ERROR == WSACleanup())
	{
		printf("server_main: Failed to close Winsocket, error %ld.\n", WSAGetLastError());
	}


	return exit_code;
}