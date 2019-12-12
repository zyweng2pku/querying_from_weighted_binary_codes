#include<cstdio>
#include<iostream>
#include<algorithm>
#include<cstring>
#include<ctime>
#include <malloc.h>
#include<map>


using namespace std;
typedef unsigned char byte;

const int nNumQuery = 10000;
const int nNumData = 1000000;
const int nSelectivity = 1000;   // 1000 nearest neighbors for CIFAR-10
char sFileName[100];
char sFileDir[100];
char sFileFull[100];



void read_goundtruth_file(char sFileName[], int* pGround) {
	printf("file name:%s\n", sFileName);
	FILE* fp;
	fp=fopen(sFileName, "r");
    if(fp==NULL) {
	  printf("´íÎó!\n");
	  exit(1);
    }	
	
	int n;

	for (int i = 0; i < nNumQuery; i++) {
		for (int j = 0; j < nSelectivity; j++)	{
			fscanf(fp, "%d", &n);
			pGround[i * nSelectivity + j] = n;
		}
	}
	fclose(fp);	
}

void read_file(char sFileName[], int* pNeighbor, int nNumTruth, bool bPrint = false) {
	printf("file name:%s\n", sFileName);
	FILE* fp;
	fp=fopen(sFileName, "rb");
    if(fp==NULL) {
	  printf("error\n");
	  exit(1); 
    }	
	
	for (int i = 0; i < nNumQuery * nNumTruth; i++)	{	
			if (fread(&pNeighbor[i], sizeof(int), 1, fp) != 1)	{
				printf("file write error\n");
				exit(0);
			}
			if (bPrint && i < nNumTruth) {
				printf("%d ", pNeighbor[i]);
			}
	}
	if (bPrint) {
		printf("\n");
	}
	fclose(fp);
}


void cal_pre(char sFileNameOri[100], int* pNeighbor, int* pGround) {
	for (int nBits = 32; nBits <= 32; nBits *= 2) {
		for (int nNumNeighbor = 1; nNumNeighbor <= 100; nNumNeighbor *= 10) {
			sprintf(sFileDir, "%d\\\0", nBits);
			sprintf(sFileName, "%s%d_%d_%d_%d_%d\0", sFileNameOri, nBits, nNumNeighbor, int(nBits / 8), nNumData, nNumQuery); 
			strcpy(sFileFull, sFileDir);
			strcat(sFileFull, sFileName);
			read_file(sFileFull, pNeighbor, nNumNeighbor);
			
			int nGroundSt = 0;
			int nQ = 0;
			double dPre = 0;
			for (int i = 0; i < nNumQuery; i++)	{
			
				
				double dPre1 = 0;
				for (int j = 0; j < nNumNeighbor; j++)
					for (int k = 0; k < nSelectivity; k++) 
						if (pNeighbor[nQ+j]+1 == pGround[nGroundSt+k]) 	{
							dPre1++;
							break;
					}						
				dPre += dPre1 / nNumNeighbor;
				nGroundSt += nSelectivity;
				nQ += nNumNeighbor;
			}
			printf("accuracy with bit %d and neighbor %d: %.5lf\n", nBits, nNumNeighbor, dPre/ nNumQuery);
		}
	}
	
	
}

int main()
{
	int* pGround = (int*)malloc(nNumQuery * nSelectivity * sizeof(int));
	int* pNeighbor = (int*)malloc(nNumQuery * 100 * sizeof(int));
	read_goundtruth_file("data/groundtruth.txt", pGround);
	cal_pre("MIWQ\0", pNeighbor, pGround );

	
	return 0;
}