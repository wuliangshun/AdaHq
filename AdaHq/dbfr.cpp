#include "dbfr.h"


vector<string> readdbf(string dbfpath)
{
	
	vector<string> outlines;

	FILE *fdbf;
	InfoOfDbfHead      Head;
	InfoOfDbfIndex     Index[48];
	int offset[48], n;
	unsigned int    rec, field, fieldnum, i;

	
	char* buf = (char*)dbfpath.c_str();	
	if ((fdbf = fopen(buf, "rb")) == NULL) {
		fprintf(stderr, "can not fopen file [%s]!", buf);
		return outlines;
	}

	fseek(fdbf, 0L, SEEK_SET);
	fread(&Head, sizeof(InfoOfDbfHead), 1, fdbf);
	fieldnum = (Head.offset - 1) / 32 - 1;

	fseek(fdbf, 32, SEEK_SET);
	n = 0;

	for (i = 0; i<fieldnum; i++) {
		offset[i] = n;
		fread(&Index[i], sizeof(InfoOfDbfIndex), 1, fdbf);
		n = n + Index[i].width;
	}
	
	for (int i = 0; i < Head.recnum; i++)
	{
		char line[500];
		memset(line, 0, sizeof(line));
		for (int j = 0; j < fieldnum; j++)
		{
			fseek(fdbf, (long)(Head.offset + 1 + i*(long)Head.reclen + offset[j]), SEEK_SET);
			fread(buf, 1, Index[j].width, fdbf);
			buf[Index[j].width] = 0;
			//printf("[%s]", buf);
			strcat(line, buf);
			if (j != fieldnum - 1)
				strcat(line, "|");			
		}
		outlines.push_back(line);		
	}

	fclose(fdbf);
	
	return outlines;
}

int readdbf_rc(string dbfpath, int row, int col)
{
	FILE *fdbf;
	InfoOfDbfHead      Head;
	InfoOfDbfIndex     Index[48];
	int offset[48], n;
	unsigned int    rec, field, fieldnum, i;
	
	char* buf = (char*)dbfpath.c_str();

	if ((fdbf = fopen(buf, "rb")) == NULL) {
		fprintf(stderr, "can not fopen file [%s]!", buf);
		return 1;
	}

	fseek(fdbf, 0L, SEEK_SET);
	fread(&Head, sizeof(InfoOfDbfHead), 1, fdbf);
	fieldnum = (Head.offset - 1) / 32 - 1;

	fseek(fdbf, 32, SEEK_SET);
	n = 0;

	for (i = 0; i<fieldnum; i++) {
		offset[i] = n;
		fread(&Index[i], sizeof(InfoOfDbfIndex), 1, fdbf);
		n = n + Index[i].width;
	}
	if (!(0 <= row && row<Head.recnum && 0 <= col && col<fieldnum)) {
		fprintf(stderr, "row %d col %d not exists!", row, col);
		fclose(fdbf);
		return 2;
	}
	else {
		rec = row;
		field = col;
	}
	fseek(fdbf, (long)(Head.offset + 1 + rec*(long)Head.reclen + offset[field]), SEEK_SET);
	fread(buf, 1, Index[field].width, fdbf);
	buf[Index[field].width] = 0;
	printf("[%s]", buf);
	fclose(fdbf);

	return 0;
}


