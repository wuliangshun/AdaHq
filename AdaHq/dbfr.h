#ifndef DBFR_H_H
#define DBFR_H_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#pragma warning(disable:4018)
#pragma warning( disable : 4996)

#pragma pack(push,1)

using namespace std;

typedef struct HeadOfDbf {
	unsigned char  tab;
	unsigned char  year;
	unsigned char  month;
	unsigned char  day;
	unsigned int   recnum;
	unsigned short offset;
	unsigned short reclen;
	char  save[20];
} InfoOfDbfHead;

typedef struct HeadOfDbfIndex {
	unsigned char fname[11];
	unsigned char type;
	int  addr;
	unsigned char width;
	unsigned char doc;
	char save[14];
} InfoOfDbfIndex;

#pragma pack(pop)


vector<string> readdbf(string dbfpath);

int readdbf_rc(string dbfpath, int row, int col);


#endif