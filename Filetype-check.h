#define _CRT_SECURE_NO_WARNINGS 1
#include <stdint.h>
#include <stdio.h>
#include <ctype.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

const char* filetype;
uint8_t first8hex[8];
uint8_t mp4_header[8] = { 0x00, 0x00, 0x00, 0x18, 0x66, 0x74, 0x79, 0x70 };
uint8_t png_header[8] = { 0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A };
uint8_t docx_pptx_xlsx_header[8] = { 0x50, 0x4B, 0x03, 0x04, 0x14, 0x00, 0x00, 0x08 };
uint8_t pdf_header[4] = { 0x25, 0x50, 0x44, 0x46 };

void binary2hex_print(char* filename, char* buf)
{
    int i = 0, j = 0;
    unsigned char* p1 = (unsigned char*)buf;
    unsigned char* p2 = (unsigned char*)buf;

    for (i = 0; i < 8; i++)
    {
        first8hex[i] = (unsigned int)*(p1 + i);
    }
}

const char* GetFileExtenstion(const char* file_name)
{
    int file_name_len = strlen(file_name);
    file_name += file_name_len;

    const char* file_ext = NULL;
    for (int i = 0; i < file_name_len; i++)
    {
        if (*file_name == '.')
        {
            file_ext = file_name + 1;
            break;
        }
        file_name--;
    }
    return file_ext;
}


void fileheaderhexcheck()
{
    int is_mp4 = memcmp(mp4_header, first8hex, sizeof(mp4_header));
    int is_png = memcmp(png_header, first8hex, sizeof(png_header));
    int is_mcsftexe = memcmp(docx_pptx_xlsx_header, first8hex, sizeof(docx_pptx_xlsx_header));
    int is_pdf = memcmp(pdf_header, first8hex, sizeof(pdf_header));

    if (is_mp4 == 0)
    {
        filetype = "mp4";
    }
    else if (is_png == 0)
    {
        filetype = "png";
    }
    else if (is_mcsftexe == 0)
    {
        filetype = "miscrosoft exe file";
    }
    else if (is_pdf == 0)
    {
        filetype = "pdf";
    }
    else
    {
        filetype = "error";
    }
}