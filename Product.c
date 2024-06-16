#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "Product.h"
#include "General.h"
#include "fileHelper.h"


#define MIN_DIG 3
#define MAX_DIG 5

void	initProduct(Product* pProduct)
{
	initProductNoBarcode(pProduct);
	getBorcdeCode(pProduct->barcode);
}

void	initProductNoBarcode(Product* pProduct)
{
	initProductName(pProduct);
	pProduct->type = getProductType();
	pProduct->price = getPositiveFloat("Enter product price\t");
	pProduct->count = getPositiveInt("Enter product number of items\t");
}

void initProductName(Product* pProduct)
{
	do {
		printf("enter product name up to %d chars\n", NAME_LENGTH );
		myGets(pProduct->name, sizeof(pProduct->name),stdin);
	} while (checkEmptyString(pProduct->name));
}

void	printProduct(const Product* pProduct)
{
	printf("%-20s %-10s\t", pProduct->name, pProduct->barcode);
	printf("%-20s %5.2f %10d\n", typeStr[pProduct->type], pProduct->price, pProduct->count);
}

int		saveProductToFile(const Product* pProduct, FILE* fp)
{
	if (fwrite(pProduct, sizeof(Product), 1, fp) != 1)
	{
		puts("Error saving product to file\n");
		return 0;
	}
	
	return 1;
}

int	 saveProductToFileCompressed( Product* pProduct, FILE* fp)
{
	int len =  (int)strlen(pProduct->name);
	for (int i = 0; i < 7; i++)
	{
		pProduct->barcode[i] = castingToWrite(pProduct->barcode[i]);
	}

	BYTE data1[3];
	data1[0] = (pProduct->barcode[0] << 2) | (pProduct->barcode[1] >> 4);
	data1[1] = (pProduct->barcode[1] << 4) | ((pProduct->barcode[2] & 0x3C) >> 2);
	data1[2] = (pProduct->barcode[2] << 6) |  pProduct->barcode[3];

	BYTE data2[3];
	data2[0] = (pProduct->barcode[4] << 2) | (pProduct->barcode[5] >> 4);
	data2[1] = (pProduct->barcode[5] << 4) | (pProduct->barcode[6] >> 2);
	data2[2] = (pProduct->barcode[6] << 6) | ((len & 0xF) << 2) | (pProduct->type & 0x3);

	if (fwrite(&data1, sizeof(BYTE), 3, fp) != 3)
		return 0;

	if (fwrite(&data2, sizeof(BYTE), 3, fp) != 3)
		return 0;

	if (!writeCharsToFile(pProduct->name, len, fp, "Error write product name"))
		return 0;


	BYTE data3[3];
	data3[0] = (pProduct->count & 0xFF);
	float floatPrice = pProduct->price;
	int integerPrice = (int)floatPrice;
	int priceAfterDot = (int)((floatPrice - integerPrice) * 100);

	data3[1] = (priceAfterDot << 1 ) | ((integerPrice >> 8) & 0x1) ;
	data3[2] = (integerPrice & 0xFF);

	if (fwrite(&data3, sizeof(BYTE), 3, fp) != 3)
		return 0;

	return 1;
}
int	 loadProductFromFileCompressed(Product* pProduct, FILE* fp)
{
	BYTE data1[3] = { 0 };
	BYTE data2[3] = { 0 };
	BYTE data3[3] = { 0 };

	if (fread(&data1, sizeof(BYTE),3, fp) != 3)
		return 0;
	if (fread(&data2, sizeof(BYTE), 3, fp) != 3)
		return 0;

	 pProduct->barcode[0] = (data1[0] >> 2) ;
	 pProduct->barcode[1] = ((data1[0] & 0x3) << 4) | (data1[1] >> 4);
	 pProduct->barcode[2] = ((data1[1] & 0xF) << 2) | ((data1[2] >> 6) & 0x3);
	 pProduct->barcode[3] = (data1[2] & 0x3F);
	 pProduct->barcode[4] = (data2[0] >> 2) & 0x3F;
	 pProduct->barcode[5] = ((data2[0] & 0x3) << 4) | ((data2[1] >> 4) & 0xF);
	 pProduct->barcode[6] = ((data2[1] & 0xF) << 2 )| ((data2[2] >> 6) & 0x3);
	
	for (int i = 0; i < 7; i++)
	{
		pProduct->barcode[i] = castingToRead(pProduct->barcode[i]);
	}

	int name_len = (int)((data2[2] >> 2) & 0xF);

	pProduct->type = (eProductType)(data2[2] & 0x3);

	if (fread(pProduct->name, sizeof(char), name_len, fp) != name_len)
		return 0;

	if (fread(&data3, sizeof(BYTE), 3, fp) != 3)
		return 0;
	pProduct->count = (int)data3[0];
	float agorots = (float)(((data3[1] >> 1) & 0x7F) / 100);
	int shekel = ((data3[1] & 0x1) << 8) | ((data3[2] & 0xFF));
	pProduct->price = agorots + shekel;

	return 1;
}


char castingToWrite(char c)
{
	if (c >= 65)
		c -= 55;
	else
		c -= 48;
	return c;
}
char castingToRead(char c)
{
	if (c >= 10)
		c += 55;
	else
		c += 48;
	return c;
}


int		loadProductFromFile(Product* pProduct, FILE* fp)
{
	if (fread(pProduct, sizeof(Product), 1, fp) != 1)
	{
		puts("Error reading product from file\n");
		return 0;
	}
	return 1;
}

void getBorcdeCode(char* code)
{
	char temp[MAX_STR_LEN];
	char msg[MAX_STR_LEN];
	sprintf(msg,"Code should be of %d length exactly\n"
				"UPPER CASE letter and digits\n"
				"Must have %d to %d digits\n"
				"First and last chars must be UPPER CASE letter\n"
				"For example A12B40C\n",
				BARCODE_LENGTH, MIN_DIG, MAX_DIG);
	int ok = 1;
	int digCount = 0;
	do {
		ok = 1;
		digCount = 0;
		printf("Enter product barcode "); 
		getsStrFixSize(temp, MAX_STR_LEN, msg);
		if (strlen(temp) != BARCODE_LENGTH)
		{
			puts(msg);
			ok = 0;
		}
		else {
			//check and first upper letters
			if(!isupper(temp[0]) || !isupper(temp[BARCODE_LENGTH-1]))
			{
				puts("First and last must be upper case letters\n");
				ok = 0;
			} else {
				for (int i = 1; i < BARCODE_LENGTH - 1; i++)
				{
					if (!isupper(temp[i]) && !isdigit(temp[i]))
					{
						puts("Only upper letters and digits\n");
						ok = 0;
						break;
					}
					if (isdigit(temp[i]))
						digCount++;
				}
				if (digCount < MIN_DIG || digCount > MAX_DIG)
				{
					puts("Incorrect number of digits\n");
					ok = 0;
				}
			}
		}
		
	} while (!ok);

	strcpy(code, temp);
}


eProductType getProductType()
{
	int option;
	printf("\n\n");
	do {
		printf("Please enter one of the following types\n");
		for (int i = 0; i < eNofProductType; i++)
			printf("%d for %s\n", i, typeStr[i]);
		scanf("%d", &option);
	} while (option < 0 || option >= eNofProductType);
	getchar();
	return (eProductType)option;
}

const char* getProductTypeStr(eProductType type)
{
	if (type < 0 || type >= eNofProductType)
		return NULL;
	return typeStr[type];
}

int		isProduct(const Product* pProduct, const char* barcode)
{
	if (strcmp(pProduct->barcode, barcode) == 0)
		return 1;
	return 0;
}

int		compareProductByBarcode(const void* var1, const void* var2)
{
	const Product* pProd1 = (const Product*)var1;
	const Product* pProd2 = (const Product*)var2;

	return strcmp(pProd1->barcode, pProd2->barcode);
}


void	updateProductCount(Product* pProduct)
{
	int count;
	do {
		printf("How many items to add to stock?");
		scanf("%d", &count);
	} while (count < 1);
	pProduct->count += count;
}


void	freeProduct(Product* pProduct)
{
	//nothing to free!!!!
}