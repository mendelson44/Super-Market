#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fileCompressed.h"
#include "fileHelper.h"

int	 writeSuperNameAndProductCountCompressed(int val, const char* str, FILE* fp, const char* msg)
{
	BYTE data[2] = { 0 };

	int len = (int)strlen(str);
	data[0] = val >> 2;
	data[1] = (val & 0x3) << 6 | (len & 0x3F);

	if (fwrite(&data, sizeof(BYTE), 2, fp) != 2)
		return 0;


	if (!writeCharsToFile(str, len, fp, msg))
		return 0;

	return 1;
}
int	 readNameAndProductCountCompressed(int* val, char** str, FILE* fp, const char* msg)
{
	BYTE data[2] = { 0 };
	int len;
	if (fread(&data, sizeof(BYTE), 2, fp) != 2)
		return 0;

	*val = data[0] << 2 | ((data[1] >> 6) & 0x3);
	len = data[1] & 0x3F;
	*str = (char*)calloc(len+1,sizeof(char));
	if (!(*str))
	{
		puts(msg);
		return 0;
	}
	if(!(readCharsFromFile(*str,len,fp,msg)))
		return 0;

	return 1;

}

void printCharAsBinary(BYTE ch)
{
	int i;
	BYTE temp;
	for (i = 0; i < 8; i++)
	{
		temp = ch << i;
		temp = temp >> 7;
		printf("%d", (int)temp);
	}
}

BYTE createMask(int high, int low)
{
	return (1 << (high + 1)) - (1 << low);
}
	

