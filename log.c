#include <stdio.h>
#include <time.h>
#include <stdarg.h>
FILE* log_file = NULL;
const char* DEFAULT = "log.txt";
int LOG(const char* f, ...)
{
	va_list va;
	va_start(va,f);
	if(!log_file)
	{
		log_file = fopen(DEFAULT, "a");
	}
	if(log_file)
	{
		char timeNow[100];
		time_t timeVal = time(NULL);
		struct tm *locTime = localtime(&timeVal);
		strftime(timeNow,100,"[%T] ",locTime);
		fprintf(log_file,timeNow);
		vfprintf(log_file, f, va);
		fflush(log_file);
	}
	va_end(va);
	return 0;
}
