#pragma once
typedef unsigned char BYTE;
int writeSuperNameAndProductCountCompressed(int val, const char* str, FILE* fp, const char* msg);
int	 readNameAndProductCountCompressed(int* val, char** str, FILE* fp, const char* msg);
void printCharAsBinary(BYTE ch);
BYTE createMask(int high, int low);
