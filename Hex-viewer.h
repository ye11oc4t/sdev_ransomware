#pragma once
#define _CRT_SECURE_NO_WARNINGS 1

#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <stdlib.h>

void binary2hex_print(char* filename, char* buf, int len)
{
    int i = 0, j = 0;
    unsigned char* p1 = (unsigned char*)buf;
    unsigned char* p2 = (unsigned char*)buf;

    printf("+++ %s (%s:%d)\n", "mainCodes", filename, len);
    printf("----------------------------------------------------------------\n");
    printf("++++++++++++++++ BINARY HEX +++++++++++++++++++ ++ ASCII +++++++\n");
    printf("----------------------------------------------------------------\n");
    for (i = 0; i < len; i++)
    {
        printf("%02x ", (unsigned int)*(p1 + i));
        if (((i + 1) % 16 == 0) && (i > 0))
        {
            for (j = 0; j < 16; j++)
            {
                if (isprint(*(p2 + j)) && (iscntrl(*(p2 + j)) == 0))
                    printf("%c", *(p2 + j));
                else
                    printf(".");
            }
            printf("\n");
            p2 = p1 + i + 1;
        }
    }

    if ((((i + 1) % 16) <= 15) && ((i + 1) % 16 != 1))
    {
        for (j = (i + 1) % 16 - 1; j < 16; j++)
            printf("   ");

        for (j = 0; j < (i + 1) % 16 - 1; j++)
        {
            if (isprint(*(p2 + j)) && (iscntrl(*(p2 + j)) == 0))
                printf("%c", *(p2 + j));
            else
                printf(".");
        }
        printf("\n");
    }
    printf("----------------------------------------------- ----------------\n");
}


#define FILE_NAME "pthreadVC2_part.lib"
/*
int main()
{
    FILE* fp = NULL;
    int size = 0;
    char* read_buf = NULL;

    fp = fopen(FILE_NAME, "rb"); //바이너리 읽기 모드
    if (fp == NULL)
    {
        printf("open error\n");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    read_buf = (char*)calloc(size + 1, sizeof(char));
    fread(read_buf, sizeof(char), size, fp);
    fclose(fp);

    //바이너리 파일 화면 출력
    binary2hex_print((char*)FILE_NAME, read_buf, size);

    return 0;
}


*/