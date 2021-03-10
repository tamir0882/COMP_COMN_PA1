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

#define IN_PORT 8080





int main(int argc, char* argv[])
{
	// initialize windows networking
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != NO_ERROR)
		printf("Error at WSAStartup()\n");


	SOCKADDR_IN server_addr;


	// create the socket that will listen for incoming TCP connections
	SOCKET s_udp_listen = socket(AF_INET, SOCK_DGRAM, 0);
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(IN_PORT);
	
	int status = bind(s_udp_listen, (SOCKADDR*)&server_addr, sizeof(server_addr));
	status = listen(s_udp_listen, SOMAXCONN);
	
	
	while (1)
	{
		sscanf
		//here need to change for udp communication
		received = recvfrom(s, recieved_buffer, MSG_SIZE, 0);
		if (received)
			printf("%s", recieved_buffer);
	}
	return 0;
}