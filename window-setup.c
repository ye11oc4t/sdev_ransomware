#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <Lm.h>
#pragma comment(lib, "netapi32.lib")
#pragma warning(disable:4996)


const int* GetOSName(DWORD dwMajor, DWORD dwMino) {

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
	else if ((dwMajor == 10) && (dwMino == 20)) return "WIN_10,11";
	return "WIN_ETC";
}

int main() {
	static DWORD dwMajorCache = 0, dwMinorCache = 0;
	LPWKSTA_INFO_100 pBuf = NULL;
	if (NERR_Success != NetWkstaGetInfo(NULL, 100, (LPBYTE*)&pBuf))
		return FALSE;
	dwMajorCache = pBuf->wki100_ver_major;
	dwMinorCache = pBuf->wki100_ver_minor; NetApiBufferFree(pBuf);
	
	printf("version : %s\n", GetOSName(dwMajorCache, dwMinorCache));
}