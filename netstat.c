#include "netstat.h"
#include <stdio.h>
#include <string.h>


// module-only global file handle
FILE* dataSource = NULL;


// get the count of 'c' occurences in str
int stroccur(char* str, char c)
{
	int count = 0;
	while(*str != '\0')
	{
		if(*(str++) == c)
			count++;
	}
	return count;
}
// Convert ascii address to port
int column2port(char* column)
{
	char* port = strrchr(column,':')+1;
	if(port == 1)
		return 0;
	return atoi(port);
}
void skipSpaces(FILE* file)
{
	int c;
	while((c = fgetc(file)) != EOF)
	{
		if(!isspace(c))
		{
			ungetc(c,file);
			return;
		}
	}
}
// Read a single column until end of line
int readColumnEOL(FILE* file, char* column)
{
	int c,lastc = 'a';
	do {
		c = fgetc(file);
		if(c == '\n')
			break;
		if(!(isspace(c) && isspace(lastc)))
		{
			*(column++) = c;
			lastc = c;
		}

	} while(c != EOF);
	*column = '\0';
	return 0;
}
// Read a single column from line 
int readColumn(FILE* file, char* column)
{
	int c;
	do {
		c = fgetc(file);
		if(c == '\n')
			break;
		if(isspace(c))
		{
			skipSpaces(file);
			break;
		} else {
			*(column++) = c;
		}
		
	} while(c != EOF);
	*column = '\0';
	return 0;
}

/*
 * 	PUBLIC FUNCTIONS
 */

void n_init()
{
	dataSource = NULL;
}

int n_getData(ndata* data)
{
	int c = getc(dataSource);
	if(c == EOF)
		return 0;
	ungetc(c,dataSource);
	char col[256];
	int colnum = 0;
	int port;
	int isUdp;
	int isIP6;

	// proto
	readColumn(dataSource,col);
	if(strlen(col) == 0)
		return 0;
	isUdp = strcmp(col,"tcp");
	// unk
	readColumn(dataSource,col);
	// unk
	readColumn(dataSource,col);
	// src
	readColumn(dataSource,col);
	if(strlen(col) == 0)
		return 0;
	// if more than one ':' occurs in local address => IPv6
	isIP6 = (stroccur(col, ':') > 1);

	port = column2port(col);
	
	// dest
	readColumn(dataSource,col);
	// state (skipped in case of UDP)
	if(!isUdp)
	{
		readColumn(dataSource,col);
	}
	// program (root access required)
	readColumnEOL(dataSource,col);

	// fill provided data	
	data->proto = isUdp;
	data->port = port;
	if(col[3] != '-')
		strcpy(data->program, col);
	else
		strcpy(data->program, "-");
	return 1;
}

int n_load()
{

	if(dataSource!= NULL)
		pclose(dataSource);
	dataSource = popen("netstat -tuapn --numeric-ports 2>/dev/null| tail -n+3","r");
	if(dataSource)
		return 1;
	return 0;
}

void n_dtor()
{
	pclose(dataSource);
	dataSource = NULL;
}
