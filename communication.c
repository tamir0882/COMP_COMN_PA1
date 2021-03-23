
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




int send_data(const char* buffer, int bytes_to_send, SOCKET s_send, SOCKADDR_IN recv_addr)
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

	return bytes_transferred;
}

int recv_data(char* buffer_recv, int buf_size, SOCKET s_recv, SOCKADDR_IN* sender_addr)
{
	if (NULL == buffer_recv)
	{
		printf("inside recv_data, buffer_recv was NULL prt.\n");
		return FAILURE;
	}

	int recv_bytes = 0;
	
	int send_addr_size = sizeof(*sender_addr);

	printf("inside recv_data, attempting recvfrom.\n");

	recv_bytes = recvfrom(s_recv, buffer_recv, buf_size, 0, (SOCKADDR*)sender_addr, &send_addr_size);
	if (recv_bytes == SOCKET_ERROR) 
	{
		printf("recvfrom failed with error %d\n", WSAGetLastError());
		return FAILURE;
	}

	return recv_bytes;
}


void printBits(size_t const size, void const* const ptr)
{
	unsigned char* b = (unsigned char*)ptr;
	unsigned char byte;
	int i, j;

	for (i = size - 1; i >= 0; i--) {
		for (j = 7; j >= 0; j--) {
			byte = (b[i] >> j) & 1;
			printf("%u", byte);
		}
	}
	puts("");
}


void encode_hamming(unsigned short* us_word)
{
	// Code Generating Matrix.
	char matrix_G[DW_BIT_SIZE][CW_BIT_SIZE] = {
						{ 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
						{ 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 1, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0 },
						{ 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
						{ 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 },
						{ 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0 },
						{ 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 },
						{ 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0 },
						{ 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1 } };

	unsigned short result_code_word = 0;

	for (int j = 0; j < CW_BIT_SIZE; j++) {
		for (int k = 0; k < DW_BIT_SIZE; k++) {
			result_code_word ^= (((*us_word >> k) & 0x1) * matrix_G[k][j]) << j;
		}
	}

	*us_word = result_code_word;
}


void decode_hamming(unsigned short* us_word)
{
	// Code Generating Matrix.
	char matrix_G[CW_BIT_SIZE][DW_BIT_SIZE] = {
						{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
						{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 } };


	unsigned short result_data_word = 0;

	// Multiplying first and second matrices and storing it in result
	for (int j = 0; j < DW_BIT_SIZE; j++) {
		for (int k = 0; k < CW_BIT_SIZE; k++) {
			result_data_word ^= (((*us_word >> k) & 0x1) * matrix_G[k][j]) << j;
		}
	}

	*us_word = result_data_word;
}


int error_check(unsigned short code_word)
{
	char matrix_H[CW_BIT_SIZE][POS_BIT_SIZE] = {
						{ 1, 0, 0, 0 },
						{ 0, 1, 0, 0 },
						{ 1, 1, 0, 0 },
						{ 0, 0, 1, 0 },
						{ 1, 0, 1, 0 },
						{ 0, 1, 1, 0 },
						{ 1, 1, 1, 0 },
						{ 0, 0, 0, 1 },
						{ 1, 0, 0, 1 },
						{ 0, 1, 0, 1 },
						{ 1, 1, 0, 1 },
						{ 0, 0, 1, 1 },
						{ 1, 0, 1, 1 },
						{ 0, 1, 1, 1 },
						{ 1, 1, 1, 1 } };

	char error_pos = 0;

	// Multiplying first and second matrices and storing it in result
	for (int j = 0; j < POS_BIT_SIZE; j++) {
		for (int k = 0; k < CW_BIT_SIZE; k++) {
			error_pos ^= (((code_word >> k) & 0x1) * matrix_H[k][j]) << j;
		}
	}

	return error_pos;

}


int check_and_fix_hamming(unsigned short* code_word)
{
	char pos = 0;
	unsigned short fix = 0x1;

	pos = error_check(*code_word);

	if (pos)
	{
		fix <<= (pos - 1);
		*code_word ^= fix;
	}

	return pos;
}


void str2dw(char* data, unsigned short* data_word)
{
	unsigned long long help_long[2] = { 0,0 };
	memcpy(help_long, (unsigned char*)data, DW_BIT_SIZE);

	int index = 0;

	for (int i = 0; i < NUM_OF_SHORTS; i++)
	{
		data_word[i] = 0;
	}

	for (int i = 0; i < NUM_OF_SHORTS; i++)
	{
		index = i / 6;
		if (i == 5)
		{
			data_word[i] = help_long[index] & 0x1ff;
			data_word[i] |= ((help_long[index + 1] & 0x3) << 9);
			help_long[index + 1] >>= 2;
		}
		else
		{
			data_word[i] = help_long[index] & 0x7ff;
			help_long[index] >>= DW_BIT_SIZE;
		}
	}
}


void dw2str(char* data, unsigned short* data_word)
{
	for (int i = 0; i < DW_BIT_SIZE; i++)
	{
		data[i] = 0;
	}

	for (int i = 0; i < NUM_OF_SHORTS; i++)
	{
		if (i % 2)
			continue;

		data_word[i] <<= 5;
	}


	int help_int[4] = { 0,0,0,0 };

	memcpy(&help_int, data_word, 16);

	int index = 0;

	for (int i = 0; i < 4; i++)
	{
		help_int[i] >>= 5;
	}

	for (int i = 0; i < DW_BIT_SIZE; i++)
	{
		index = i / 3;
		if (i == 2)
		{
			data[i] = help_int[index] & 0x3f;
			data[i] |= ((help_int[index + 1] & 0x3) << 6);
			help_int[index + 1] >>= 2;
		}
		else if (i == 5)
		{
			data[i] = help_int[index] & 0x0f;
			data[i] |= ((help_int[index + 1] & 0x0f) << 4);
			help_int[index + 1] >>= 4;
		}
		else if (i == 8)
		{
			data[i] = help_int[index] & 0x03;
			data[i] |= ((help_int[index + 1] & 0x3f) << 2);
			help_int[index + 1] >>= 6;
		}
		else
		{
			data[i] = help_int[index] & 0xff;
			help_int[index] >>= 8;
		}
	}
}


void str2cw(char* data, unsigned short* code_word)
{
	unsigned long long help_long[2] = { 0,0 };

	memcpy(&help_long, (unsigned char*)data, CW_BIT_SIZE);

	for (int i = 0; i < NUM_OF_SHORTS; i++)
	{
		code_word[i] = 0;
	}

	int index = 0;

	for (int i = 0; i < NUM_OF_SHORTS; i++)
	{
		index = i / 5;
		if (i == 4)
		{
			code_word[i] = help_long[index] & 0xf;
			code_word[i] |= ((help_long[index + 1] & 0x7ff) << 4);
			help_long[index + 1] >>= 11;
		}
		else
		{
			code_word[i] = help_long[index] & 0x7fff;
			help_long[index] >>= CW_BIT_SIZE;
		}
	}
}


void cw2str(char* data, unsigned short* code_word)
{
	for (int i = 0; i < DW_BIT_SIZE; i++)
	{
		data[i] = 0;
	}

	for (int i = 0; i < NUM_OF_SHORTS; i++)
	{
		if (i % 2)
			continue;

		code_word[i] <<= 1;
	}


	int help_int[4] = { 0,0,0,0 };

	memcpy(&help_int, code_word, 16);

	int index = 0;

	for (int i = 0; i < 4; i++)
	{
		help_int[i] >>= 1;
	}

	for (int i = 0; i < CW_BIT_SIZE; i++)
	{
		index = i / 4;
		if (i == 3)
		{
			data[i] = help_int[index] & 0x3f;
			data[i] |= ((help_int[index + 1] & 0x3) << 6);
			help_int[index + 1] >>= 2;
		}
		else if (i == 7)
		{
			data[i] = help_int[index] & 0x0f;
			data[i] |= ((help_int[index + 1] & 0x0f) << 4);
			help_int[index + 1] >>= 4;
		}
		else if (i == 11)
		{
			data[i] = help_int[index] & 0x03;
			data[i] |= ((help_int[index + 1] & 0x3f) << 2);
			help_int[index + 1] >>= 6;
		}
		else
		{
			data[i] = help_int[index] & 0xff;
			help_int[index] >>= 8;
		}

	}
}


int generate_coded_str(char* data, size_t* buf_size)
{
	if (*buf_size % DW_BIT_SIZE)
	{
		printf("generate_coded_str failure, buf_size must be devidable by 11.\n");
		return FAILURE;
	}

	unsigned short* us_word = NULL;
	int dw_array_size = 0;

	int loop_iters = 0;
	loop_iters = *buf_size / DW_BIT_SIZE;

	dw_array_size = NUM_OF_SHORTS * loop_iters;

	us_word = (unsigned short*)calloc(dw_array_size, sizeof(unsigned short));
	if (NULL == us_word)
	{
		printf("generate_coded_str failure, calloc returned NULL ptr.\n");
		return FAILURE;
	}

	for (int i = 0; i < loop_iters; i++)
	{
		str2dw(&data[DW_BIT_SIZE * i], &us_word[NUM_OF_SHORTS * i]);
		for (int j = 0; j < NUM_OF_SHORTS; j++)
		{
			printf("data_word[%d] is: %d\n", NUM_OF_SHORTS * i + j, us_word[NUM_OF_SHORTS * i + j]);
			encode_hamming(&us_word[NUM_OF_SHORTS * i + j]);
			printf("after encoding, code_word[%d] is: %d\n\n", NUM_OF_SHORTS * i + j, us_word[NUM_OF_SHORTS * i + j]);

		}
	}

	for (int i = 0; i < loop_iters; i++)
	{
		cw2str(&data[CW_BIT_SIZE * i], &us_word[NUM_OF_SHORTS * i]);
	}

	free(us_word);

	*buf_size = CW_BIT_SIZE * loop_iters;

	return SUCCESS;
}


int decode_str(char* data, size_t* buf_size)
{
	if (*buf_size % CW_BIT_SIZE)
	{
		printf("generate_coded_str failure, buf_size must be devidable by 11.\n");
		return FAILURE;
	}

	int fix_count = 0;

	unsigned short* us_word = NULL;
	int dw_array_size = 0;

	int loop_iters = 0;
	loop_iters = *buf_size / CW_BIT_SIZE;

	dw_array_size = NUM_OF_SHORTS * loop_iters;

	us_word = (unsigned short*)calloc(dw_array_size, sizeof(unsigned short));
	if (NULL == us_word)
	{
		printf("decode_str failure, calloc returned NULL ptr.\n");
		return FAILURE;
	}



	for (int i = 0; i < loop_iters; i++)
	{
		str2cw(&data[CW_BIT_SIZE * i], &us_word[i * NUM_OF_SHORTS]);

		printf("after str2cw, code_word[%d] is: %d\n\n", i * NUM_OF_SHORTS, us_word[i * NUM_OF_SHORTS]);

		for (int j = 0; j < NUM_OF_SHORTS; j++)
		{
			if (check_and_fix_hamming(&us_word[i * NUM_OF_SHORTS + j]))
			{
				fix_count++;
			}

			decode_hamming(&us_word[i * NUM_OF_SHORTS + j]);;

			printf("after decoding, data_word[%d] is: %d\n", i * NUM_OF_SHORTS + j, us_word[i * NUM_OF_SHORTS + j]);
		}
	}


	for (int i = 0; i < loop_iters; i++)
	{
		dw2str(&data[DW_BIT_SIZE * i], &us_word[NUM_OF_SHORTS * i]);
	}

	free(us_word);

	*buf_size = loop_iters * DW_BIT_SIZE;

	return fix_count;
}

