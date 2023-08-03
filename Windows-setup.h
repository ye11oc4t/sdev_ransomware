#pragma once
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
using namespace std;
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

const char* GetOSName() {
    static DWORD dwMajor = 0, dwMinor = 0;

    char* osBuildNumber;

    osBuildNumber = (char*)malloc(100);
    memset(osBuildNumber, 0, 100);
    //
    LPWKSTA_INFO_100 pBuf = NULL;
    if (NERR_Success != NetWkstaGetInfo(NULL, 100, (LPBYTE*)&pBuf))
        return FALSE;
    dwMajor = pBuf->wki100_ver_major;
    dwMinor = pBuf->wki100_ver_minor;
    NetApiBufferFree(pBuf);

    int count = 0;
    char* result;
    char Window_version[1024];

    //cmd에서 버전정보 긁어오는 코드
    FILE* fp = _popen("cmd /c ver", "r");


    fgetc(fp);
    fgets(Window_version, 1024, fp);
    fclose(fp);

    //긁어온 버전정보에서 10, 11구분위해 buildnumber쪽만 추출하는 부분
    result = strtok(Window_version, "\.");

    while (result != NULL)
    {
        result = strtok(NULL, "\.");
        count += 1;
        if ((count == 2)) {
            strcat(osBuildNumber, result);
        }
    }

    int buildNumber = atoi(osBuildNumber);

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
    if ((dwMajor == 4) && (dwMinor == 0)) return "WIN_95";
    else if ((dwMajor == 4) && (dwMinor == 10)) return "WIN_98";
    else if ((dwMajor == 4) && (dwMinor == 90)) return "WIN_ME";
    else if ((dwMajor == 5) && (dwMinor == 0)) return "WIN_2000";
    else if ((dwMajor == 5) && (dwMinor == 1)) return "WIN_XP";
    else if ((dwMajor == 5) && (dwMinor == 2)) return "WIN_.NET";
    else if ((dwMajor == 6) && (dwMinor == 0)) return "WIN_VISTA";
    else if ((dwMajor == 6) && (dwMinor == 1)) return "WIN_7";
    else if ((dwMajor == 6) && (dwMinor == 2)) return "WIN_8";
    else if ((dwMajor == 6) && (dwMinor == 3)) return "WIN_8.1";
    else if ((dwMajor == 10) && (dwMinor == 0)) {
        if (buildNumber < 22000) {
            return "WIN_10";
        }
        else if (buildNumber > 22000) {
            return "WIN_11";
        }

    }
    return "WIN_ETC";
}