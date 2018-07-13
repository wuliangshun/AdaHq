#include "tool.h"



vector<char*> splitbuff(char* buff, char* d)
{	
	vector<char*> vec;
	char *p;
	p = strtok(buff, d);
	while (p != NULL)
	{
		vec.push_back(p);
		p = strtok(NULL, d);
	}
	return vec;
};


char* ltrim_lc(char* s)
{
	char *p, *q;
	for (p = q = s; *p != '\0';)
	{
		if (*p == ' ')
			++p;
		else
			*q++ = *p++;
	}
	*q = '\0';
	return s;
};



