#define  _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Address.h"
#include "General.h"
#include "fileHelper.h"
#include "SuperFile.h"
#include "myMacros.h"
#include "fileCompressed.h"



int	saveSuperMarketToFile(const SuperMarket* pMarket, const char* fileName,
	const char* customersFileName , int fileProcess)
{
	FILE* fp;
	fp = fopen(fileName, "wb");
	CHECK_MSG_RETURN_0(fp);
	if (fileProcess)
	{
		int count = getNumOfProductsInList(pMarket);
		if (!writeSuperNameAndProductCountCompressed(count, pMarket->name, fp, "Error write supermarket name and product count Compressed\n"))
		{
			CLOSE_RETURN_0(fp);
		}

		if (!saveAddressToFileCompressed(&pMarket->location, fp))
		{
			CLOSE_RETURN_0(fp);
		}

	}
	else
	{
		if (!writeStringToFile(pMarket->name, fp, "Error write supermarket name\n"))
		{
			CLOSE_RETURN_0(fp);
		}

		if (!saveAddressToFile(&pMarket->location, fp))
		{
			CLOSE_RETURN_0(fp);
		}
		int count = getNumOfProductsInList(pMarket);

		if (!writeIntToFile(count, fp, "Error write product count\n"))
		{
			CLOSE_RETURN_0(fp);
		}
	}

	Product* pTemp;
	NODE* pN = pMarket->productList.head.next; //first Node
	while (pN != NULL)
	{
		pTemp = (Product*)pN->key;
		if (fileProcess)
		{
			if (!saveProductToFileCompressed(pTemp, fp))
			{
				CLOSE_RETURN_0(fp);
			}
		}
		else
		{
				if (!saveProductToFile(pTemp, fp))
				{
					CLOSE_RETURN_0(fp);
				}
		}
		pN = pN->next;
	}

	fclose(fp);

	saveCustomerToTextFile(pMarket->customerArr,pMarket->customerCount, customersFileName);

	return 1;
}
#define BIN
#ifdef BIN
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName, int fileProcess)
{
	FILE* fp;
	fp = fopen(fileName, "rb");

	CHECK_MSG_RETURN_0(fp);
	int count;

	if (fileProcess)
	{
		if (!readNameAndProductCountCompressed(&count, &(pMarket->name), fp, "Error read supermarket name and product count Compressed\n"))
		{
			CLOSE_RETURN_0(fp);
		}

		if (!readAddressFromFileCompressed(&pMarket->location, fp))
		{
			CLOSE_RETURN_0(fp);
		}
	}
	else
	{
		pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
		if (!pMarket->name)
		{
			CLOSE_RETURN_0(fp);
		}

		if (!loadAddressFromFile(&pMarket->location, fp))
		{
			FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
		}

		if (!readIntFromFile(&count, fp, "Error reading product count\n"))
		{
			FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
		}
	}
	
	if (count > 0)
	{
		Product* pTemp;
		for (int i = 0; i < count; i++)
		{
			pTemp = (Product*)calloc(1, sizeof(Product));
			if (!pTemp)
			{
				printf("Allocation error\n");
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}
			if (fileProcess)
			{
				if (!loadProductFromFileCompressed(pTemp, fp))
				{
					L_free(&pMarket->productList, freeProduct);
					FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
				}
			}
			else
			{
				if (!loadProductFromFile(pTemp, fp))
				{
					L_free(&pMarket->productList, freeProduct);
					FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
				}
			}
			if (!insertNewProductToList(&pMarket->productList, pTemp))
			{
				L_free(&pMarket->productList, freeProduct);
				FREE_CLOSE_FILE_RETURN_0(pMarket->name, fp);
			}
		}
	}
	fclose(fp);

	pMarket->customerArr = loadCustomerFromTextFile(customersFileName,&pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#else
int	loadSuperMarketFromFile(SuperMarket* pMarket, const char* fileName,
	const char* customersFileName)
{
	FILE* fp;
	fp = fopen(fileName, "rb");
	if (!fp)
	{
		printf("Error open company file\n");
		return 0;
	}

	//L_init(&pMarket->productList);


	pMarket->name = readStringFromFile(fp, "Error reading supermarket name\n");
	if (!pMarket->name)
	{
		fclose(fp);
		return 0;
	}

	if (!loadAddressFromFile(&pMarket->location, fp))
	{
		free(pMarket->name);
		fclose(fp);
		return 0;
	}

	fclose(fp);

	loadProductFromTextFile(pMarket, "Products.txt");


	pMarket->customerArr = loadCustomerFromTextFile(customersFileName, &pMarket->customerCount);
	if (!pMarket->customerArr)
		return 0;

	return	1;

}
#endif

int		loadProductFromTextFile(SuperMarket* pMarket, const char* fileName)
{
	FILE* fp;
	//L_init(&pMarket->productList);
	fp = fopen(fileName, "r");
	int count;
	fscanf(fp, "%d\n", &count);


	//Product p;
	Product* pTemp;
	for (int i = 0; i < count; i++)
	{
		pTemp = (Product*)calloc(1, sizeof(Product));
		myGets(pTemp->name, sizeof(pTemp->name), fp);
		myGets(pTemp->barcode, sizeof(pTemp->barcode), fp);
		fscanf(fp, "%d %f %d\n", &pTemp->type, &pTemp->price, &pTemp->count);
		insertNewProductToList(&pMarket->productList, pTemp);
	}

	fclose(fp);
	return 1;
}