#ifndef COMMUNICATION_H
#define COMMUNICATION_H


#define FAILURE -1
#define SUCCESS 0

#define DW_BIT_SIZE 11
#define CW_BIT_SIZE 15
#define POS_BIT_SIZE 4

#define NUM_OF_SHORTS 8

#define BYTES_IN_CODED_STR 1500
#define BYTES_IN_DEC_STR 1100

int send_data(const char* buffer, int bytes_to_send, SOCKET s_send, SOCKADDR_IN recv_addr);


//recv_data doc:
//buffer_recv = buffer to store received data
//buf_size = size of buffer_recv
//SOCKET s_recv = socket of receiver
//SOCKADDR_IN *sender_addr = address of sender
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
int recv_data(char* buffer_recv, int buf_size, SOCKET s_recv, SOCKADDR_IN *sender_addr);


void printBits(size_t const size, void const* const ptr);


void encode_hamming(unsigned short* us_word);


void decode_hamming(unsigned short* us_word);


int error_check(unsigned short code_word);


int check_and_fix_hamming(unsigned short* code_word);


void str2dw(char* data, unsigned short* data_word);

void dw2str(char* data, unsigned short* data_word);

void str2cw(char* data, unsigned short* code_word);

void cw2str(char* data, unsigned short* code_word);

int generate_coded_str(char* data, size_t* buf_size);

int decode_str(char* data, size_t* buf_size);

#endif // COMMUNICATION_H