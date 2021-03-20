
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>

#pragma comment(lib, "Ws2_32.lib")

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


#include "communication.h"




int send_data(const char* str, SOCKET communication_socket, SOCKADDR_IN recv_addr)
{
	/* Send the the request to the server on socket sd */
	int total_string_size = strlen(str) + 1;

	return send_buffer(str, total_string_size, communication_socket, recv_addr);
}



int send_buffer(const char* buffer, int bytes_to_send, SOCKET s_send, SOCKADDR_IN recv_addr)
{
	const char* p_cur_place = buffer;
	int bytes_transferred = 0;
	int bytes_left_to_send = bytes_to_send;


	while (bytes_left_to_send > 0)
	{
		/* send does not guarantee that the entire message is sent */
		bytes_transferred = sendto(s_send, p_cur_place, bytes_left_to_send, 0, (SOCKADDR*)&recv_addr, sizeof(recv_addr));

		if (bytes_transferred == SOCKET_ERROR)
		{
			printf("sendto() failed, error %d\n", WSAGetLastError());
			return FAILURE;
		}

		bytes_left_to_send -= bytes_transferred;
		p_cur_place += bytes_transferred; // <ISP> pointer arithmetic

		printf("inside send_buffer, sent %d bytes.\n", bytes_transferred);
	}

	return SUCCESS;
}


//buffer_recv = buffer to store received data
//buf_size = size of buffer_recv
//SOCKET s_recv = socket of receiver
//SOCKADDR_IN sender_addr = address of sender

//for example:
//int ret_val = 0;
//ret_val = inet_pton(AF_INET, str_ip_address, &ip_address);
//	if (1 != ret_val)
//	{
//		printf("ERROR - inet_pton failed - has failed.\n");
//		return FAILURE;
//	}
//SOCKADDR_IN sender_addr;
//sender_addr.sin_family = AF_INET;
//sender_addr.sin_addr.s_addr = ip_address;
//sender_addr.sin_port = htons(port_number);

int recv_data(char* buffer_recv, int buf_size, SOCKET s_recv, SOCKADDR_IN sender_addr)
{
	if (NULL == buffer_recv)
	{
		return FAILURE;
	}

	int recv_bytes = 0;
	int recv_res = SUCCESS;
	
	int send_addr_size = sizeof(sender_addr);

	printf("inside recv_data, attempting recvfrom.\n");

	recv_bytes = recvfrom(s_recv, buffer_recv, buf_size, 0, (SOCKADDR*)&sender_addr, &send_addr_size);
	if (recv_bytes == SOCKET_ERROR) 
	{
		printf("recvfrom failed with error %d\n", WSAGetLastError());
		return FAILURE;
	}

	return recv_res;
}

