#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#define _SVID_SOURCE
int filter(const struct dirent * file)
{
	char ptr[256];
	strncpy(ptr, file->d_name, 256);
	char* token = NULL;
	token = strtok(ptr, ".");
	if(token == NULL)
		return 0;
	if(strcmp(token, "nfcapd") == 0)
	{
		token = strtok(NULL, ".");
		while(*token != '\0')
			if(isdigit(*(token++)) == 0)
				return 0;
		//printf("File: %s\n",token);
		return 1;
	}
	return 0;
}
/* print files in current directory in reverse order */

// List nfcapd files
// Returns the count of files
int listFiles(char* dir, struct dirent ***files)
{
	int n;
   n = scandir(".", files, filter, alphasort);
	if (n < 0)
		perror("scandir");
/*
while (n--) {
            printf("%s\n", (*files)[n]->d_name);
        }
*/
	return n;
}

void freeFiles(struct dirent **namelist, int n)
{
	while (n--) {
            free(namelist[n]);
        }
        free(namelist);
}

int
main(void)
{
    /*struct dirent **namelist;
    int n;

   n = scandir(".", &namelist, filter, alphasort);
    if (n < 0)
        perror("scandir");
    else {
        while (n--) {
            printf("%s\n", namelist[n]->d_name);
            free(namelist[n]);
        }
        free(namelist);
    }	
	*/
	struct dirent **names;
	int n = listFiles(".",&names);
	for(int i = 0; i < n; i++)
		printf("%s\n", names[i]->d_name);
	freeFiles(names, n);
}

