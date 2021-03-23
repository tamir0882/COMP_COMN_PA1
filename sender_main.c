#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>

#include <ws2tcpip.h>

#include "communication.h"
#include "Sender_HardCodedData.h"

#pragma comment(lib, "Ws2_32.lib")




int main(int argc, char* argv[])
{
	if (argc != ARGC_COUNT)
	{
		printf("ARGC_COUNT fault, should receive 3 argumets with program execution.\n");
		exit(FAILURE);
	}

	int ret_val = 0;
	int exit_code = SUCCESS;

	int bytes_left_to_send = 0;
	int bytes_sent = 0;
	int pack_size = 0;

	int bytes_read_from_file = 0;
	int total_byte_count_file = 0;
	char* file_data = NULL;
	char* send_buffer = NULL;
	int data_buffer_size = 0;

	char c_read = 0;

	unsigned long channel_ip_address = 0;
	char str_channel_ip_address[17] = "";

	unsigned int channel_port = 0;

	char file_name[_MAX_PATH] = "";
	FILE* p_file = NULL;


	SOCKET s_sender = INVALID_SOCKET;

	ret_val = snprintf(str_channel_ip_address, 17, "%s", argv[IP_ADDR_INDEX]);
	if (0 >= ret_val)
	{
		printf("ERROR - snprintf for str_channel_ip_address - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	ret_val = snprintf(file_name, _MAX_PATH, "%s", argv[FILE_INDEX]);
	if (0 >= ret_val)
	{
		printf("ERROR - snprintf for file_name - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	channel_port = (unsigned int)strtol(argv[PORT_INDEX], NULL, 10);
	if (channel_port == 0)
	{
		printf("ERROR - strtol for channel_port - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	// at this point we have all needed argumets to execute the program.

	// initialize windows networking
	WSADATA wsaData;
	ret_val = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret_val != NO_ERROR)
	{
		printf("Error at WSAStartup().\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
		


	s_sender = socket(AF_INET, SOCK_DGRAM, 0);
	if (SOCKET_ERROR == s_sender)
	{
		printf("ERROR - failed to create s_sender socket - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	SOCKADDR_IN remote_addr = { .sin_addr = 0,.sin_family = 0,.sin_port = 0 };

	ret_val = inet_pton(AF_INET, str_channel_ip_address, &channel_ip_address);
	if (1 != ret_val)
	{
		printf("ERROR - inet_pton failed - has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}

	
	remote_addr.sin_family = AF_INET;
	remote_addr.sin_addr.s_addr = channel_ip_address;
	remote_addr.sin_port = htons(channel_port);


	//now we need to work with the file - read it and send it in chunks to the reciever.

	ret_val = fopen_s(&p_file, file_name, "rb");
	if (0 != ret_val || NULL == p_file)
	{
		printf("ERROR - failed to open file %s.\n", file_name);
		exit_code = FAILURE;
		goto CleanUp;
	}


	
	ret_val = fseek(p_file, 0, SEEK_END);
	if (0 != ret_val)
	{
		printf("ERROR - fseek failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	total_byte_count_file = ftell(p_file); 
	if (total_byte_count_file < 0)
	{
		printf("ERROR - ftell failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	
	rewind(p_file);

	printf("total byte count is: %d\n", total_byte_count_file);

	data_buffer_size = total_byte_count_file * 2;

	file_data = (char*)calloc(data_buffer_size, sizeof(char));
	if (NULL == file_data)
	{
		printf("ERROR - memory allocation failed for file_data.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}


	bytes_read_from_file = fread(file_data, sizeof(char), data_buffer_size, p_file);

	if (bytes_read_from_file != total_byte_count_file)
	{
		if (ferror(p_file))
		{
			exit_code = FAILURE;
			goto CleanUp;
		}
	}



	printf("successfully read %d bytes from file.\n", bytes_read_from_file);

	printf("file content is: %s\n", file_data);

	/*
	ret_val = snprintf(file_data, data_buffer_size, "%s%s", file_data, MSG_END);
	if (0 >= ret_val)
	{
		printf("ERROR - snprintf for file_data, MSG_END has failed.\n");
		exit_code = FAILURE;
		goto CleanUp;
	}
	*/

	bytes_left_to_send = bytes_read_from_file;

	send_buffer = file_data;

	while (0 < bytes_left_to_send)
	{
		if (BYTES_IN_DEC_STR < bytes_left_to_send)
		{
			pack_size = BYTES_IN_DEC_STR;
		}

		else
		{
			pack_size = bytes_left_to_send;
		}

		ret_val = generate_coded_str(send_buffer, &pack_size);

		bytes_sent = send_data(send_buffer, pack_size, s_sender, remote_addr);
		if (FAILURE == bytes_sent)
		{
			printf("ERROR - send_data failed.\n");
			exit_code = FAILURE;
			goto CleanUp;
		}

		send_buffer += bytes_sent;
		bytes_left_to_send -= bytes_sent;
	}



	//now need to wait for response from receiver about how many errors he detected.






CleanUp:
	
	if (NULL != p_file)
	{
		ret_val = fclose(p_file);
		if (0 != ret_val)
		{
			printf("ERROR - failed to close file.\n");
		}
	}
	
	
	if (NULL != file_data)
	{
		free(file_data);
	}
	

	if (INVALID_SOCKET != s_sender)
	{
		if (SOCKET_ERROR == closesocket(s_sender))
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