// visionChecksum.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ostream>


#pragma warning(disable:4996)


#define APROM_SIZE			0x8000	// for 32K APROM
#define APROM16K_SIZE		0x4000	// for 16K APROM

#define APROM_START_ADDR	0

static unsigned char binBuffer[APROM_SIZE];
unsigned int _mcuBinSize;
uint16_t _mcuBinCHECKSUM;
unsigned char *LoadUpdateBIN(char *pszIn, unsigned int *uSize)
{
	FILE *fp;
	unsigned int flen, rlen;

	if (!pszIn) {
		return (unsigned char *)0;
	}

	fp = fopen(pszIn, "rb");
	if (!fp) {
		printf("Error while opening Update BIN file !!\n");
		if (uSize) *uSize = 0;
		return (unsigned char *)0;
	}

	fseek(fp, 0L, SEEK_END);
	flen = ftell(fp);
	fseek(fp, 0L, SEEK_SET);

	//printf("!!!! BIN Size = %d\n", flen);
	if (flen > APROM_SIZE) {
		printf("Error, BIN size (0x%x) exceeds size of APROM (0x%x) !!\n", flen, APROM_SIZE);
		if (uSize) *uSize = 0;
		fclose(fp);
		return (unsigned char *)0;
	}

	memset(binBuffer, 0, APROM_SIZE);
	rlen = fread(binBuffer, 1, flen, fp);
	if (rlen != flen) {
		printf("Error while reading Update BIN file (%d != %d)!!\n", rlen, flen);
		if (uSize) *uSize = 0;
		fclose(fp);
		return (unsigned char *)0;
	}

	if (uSize) {
		*uSize = flen;
	}

	fclose(fp);
	return binBuffer;
}


int GenerateSumBIN(char *pInBin, unsigned int uSize, unsigned int checksum, char *pszOutBin)
{
	FILE *fp;
	char *aprom_bin;

	unsigned int rlen;

	if (!pszOutBin || !pInBin || !uSize) {
		return -1;
	}

	if (uSize > APROM_SIZE - 8) {
		printf("Error, BIN size (0x%x) exceeds size of APROM-8 (0x%x) !!\n", uSize, APROM_SIZE - 8);
		return -1;
	}

	fp = fopen(pszOutBin, "wb");
	if (!fp) {
		printf("Error while creating output SumBIN file, %s. !!\n", pszOutBin);
		return -1;
	}

	aprom_bin = (char *)malloc(APROM_SIZE);
	if (!aprom_bin) {
		printf("Error while allocating memory !!\n");
		if (fp) fclose(fp);
		return -1;
	}

	memset(aprom_bin, 0, APROM_SIZE);
	memcpy(aprom_bin, pInBin, uSize);
	char *pch;
	pch = aprom_bin + (APROM_SIZE - 8);
	memcpy(pch, &uSize, 4);
	pch = aprom_bin + (APROM_SIZE - 4);
	memcpy(pch, &checksum, 4);

	rlen = fwrite(aprom_bin, 1, APROM_SIZE, fp);
	if (rlen != APROM_SIZE) {
		printf("Error while writing sumBIN file (%d != %d)!!\n", rlen, APROM_SIZE);
		if (fp) fclose(fp);
		if (aprom_bin) free(aprom_bin);
		return -1;
	}

	fclose(fp);
	return 0;
}



int GenerateSumBIN16K(char *pInBin, unsigned int uSize, unsigned int checksum, char *pszOutBin)
{
	FILE *fp;
	char *aprom_bin;

	unsigned int rlen;

	if (!pszOutBin || !pInBin || !uSize) {
		return -1;
	}

	if (uSize > APROM16K_SIZE-8) {
		printf("Error, BIN size (0x%x) exceeds size of APROM-8 (0x%x) !!\n", uSize, APROM16K_SIZE -8);
		return -1;
	}

	fp = fopen(pszOutBin, "wb");
	if (!fp) {
		printf("Error while creating output SumBIN file, %s. !!\n", pszOutBin);
		return -1;
	}

	aprom_bin = (char *)malloc(APROM16K_SIZE);
	if (!aprom_bin) {
		printf("Error while allocating memory !!\n");
		if (fp) fclose(fp);
		return -1;
	}

	memset(aprom_bin, 0, APROM16K_SIZE);
	memcpy(aprom_bin, pInBin, uSize);
	char *pch;
	pch = aprom_bin + (APROM16K_SIZE - 8);
	memcpy(pch, &uSize, 4);
	pch = aprom_bin + (APROM16K_SIZE - 4);
	memcpy(pch, &checksum, 4);

	rlen = fwrite(aprom_bin, 1, APROM16K_SIZE, fp);
	if (rlen != APROM16K_SIZE) {
		printf("Error while writing sumBIN file (%d != %d)!!\n", rlen, APROM16K_SIZE);
		if (fp) fclose(fp);
		if (aprom_bin) free(aprom_bin);
		return -1;
	}

	fclose(fp);
	return 0;
}


unsigned int Checksum(const unsigned char *buf, int len)
{
	int i;
	unsigned int c;

	for (c = 0, i = 0; i < len; i++) {
		c += (unsigned int)buf[i];
	}

	return (c);
}


int main(int argc, char *argv[])
{
	unsigned int _mcuBinSize;
	unsigned int _mcuBinCHECKSUM;
	char pszOutBin16K[128];

	if (argc < 2) {
		printf("Usage : visionMcuChecksum.exe InputMcu.bin OutputMcu_SUM.bin \n");
		return -1;
	}

	char *pszInBin = argv[1];
	char *pszOutBin = argv[2];
	printf("Input: %s, Output= %s \n", pszInBin, pszOutBin);

	unsigned int uSize;
	unsigned char *pInBin;
	pInBin = LoadUpdateBIN(pszInBin, &uSize);
	if (!pInBin) {
		printf("Reading input BIN, %s, failed !!\n", pszInBin);
		return -1;
	}

	_mcuBinCHECKSUM = Checksum(pInBin, uSize);
	_mcuBinSize = uSize;
	printf("BIN Size= 0x%x (%d), Checksum = 0x%04X (%d) ...\n", _mcuBinSize, _mcuBinSize, _mcuBinCHECKSUM, _mcuBinCHECKSUM);

	//-- Generate 32K SUM BIN 
	if (remove(pszOutBin) != 0) {
		printf("Error deleting file %s\n", pszOutBin);
	}
	printf("Generating 32K SUM BIN ... %s\n", pszOutBin);
	GenerateSumBIN((char *)pInBin, uSize, _mcuBinCHECKSUM, pszOutBin);

	//-- Generate 16K SUM BIN if bin size is less than 16KB
	if (_mcuBinSize < APROM16K_SIZE - 16) {
		char *pszFileName;

		memset(pszOutBin16K, 0, sizeof(pszOutBin16K));
		pszFileName = strtok(pszOutBin, ".");
		strcpy(pszOutBin16K, pszFileName);
		strcat(pszOutBin16K, "_16K.bin");
		if (remove(pszOutBin16K) != 0) {
			printf("Error deleting file %s\n", pszOutBin16K);
		}
		printf("Generating 16K SUM BIN ... %s\n", pszOutBin16K);
		GenerateSumBIN16K((char *)pInBin, uSize, _mcuBinCHECKSUM, pszOutBin16K);
	}


    return 0;
}

