#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>


//#include "shared_HardCodedData.h"

#pragma comment(lib, "Ws2_32.lib")

#define FAILURE -1
#define SUCCESS 0
#define MSG_END "\r\n\r\n"


int send_buffer(const char* buffer, int bytes_to_send, SOCKET communication_socket, SOCKADDR_IN recv_addr);

int send_data(const char* str, SOCKET communication_socket, SOCKADDR_IN recv_addr);

int recv_data(char* buffer_recv, int buf_size, SOCKET s_recv, SOCKADDR_IN sender_addr);












#endif // COMMUNICATION_H