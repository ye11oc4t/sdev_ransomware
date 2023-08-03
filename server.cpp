#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <stdint.h>
#include <time.h>
using namespace std;

#pragma comment(lib, "ws2_32.lib")

#define FILE_MAXLEN 4096
#define BUFFER_SIZE 1024
#define AES_KEYLEN 8

#define PORT 8080

char cryptoKey[AES_KEYLEN];

void generate_key() {
	srand(time(NULL));
	for (int i = 0; i < AES_KEYLEN; i++) {
		cryptoKey[i] = rand() % 255;
	}
}

int main(void)
{
	generate_key();

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	int sock_len;
	SOCKET serverSock;
	struct sockaddr_in serverSockInfo;
	//int Readn, Writen;
	//char ReadBuffer[1024];

	serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (serverSock == INVALID_SOCKET)
		return 1;

	memset(&serverSockInfo, 0x0, sizeof(struct sockaddr_in));

	serverSockInfo.sin_family = AF_INET;
	serverSockInfo.sin_port = htons(PORT);
	serverSockInfo.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	int status = bind(serverSock, (struct sockaddr*)&serverSockInfo, sizeof(serverSockInfo));

	if (status == SOCKET_ERROR) {
		printf("Bind error\n");
		return 1;
	}
	if (SOCKET_ERROR == listen(serverSock, 5)) {
		printf("listen error\n");
		return 1;
	}

	for (int i = 0; i < AES_KEYLEN; i++)
		printf("%02x ", cryptoKey[i] & 0xFF);
	printf("\n");

	SOCKET clientSock;
	struct sockaddr_in clientSockInfo;

	memset(&clientSockInfo, 0x0, sizeof(clientSockInfo));

	int sockLen = sizeof(struct sockaddr_in);

	printf("accept wait\n");
	clientSock = accept(serverSock, (struct sockaddr*)&clientSockInfo, &sockLen);

	if (clientSock == INVALID_SOCKET) {
		printf("Accept error\n");
		closesocket(serverSock);
		WSACleanup();
		return 1;
	}

	if (send(clientSock, cryptoKey, sizeof(cryptoKey), 0) == 0) {
		fprintf(stderr, "send error\n");
		return -1;
	}

	closesocket(clientSock);
	closesocket(serverSock);
	WSACleanup();
	return 0;
}
