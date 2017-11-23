#include "CoreHelpers.h"
#include <stdlib.h>
#include <stdio.h>

#ifndef _MSC_VER
#include <time.h>
#endif

#pragma optimize("",off)

void ALWAYS_ASSERT_RAW(bool cond, const char fileName[], const int lineNum, const char funcName[], const char expression[])
{
	if (!cond)
	{
		printf("ASSERT FAILED: %s\n", expression);
		printf("File: %s\n", fileName);
		printf("Line: %d\n", lineNum);
		printf("Func: %s\n", funcName);
		DebugBreak();
	}
}

std::string GetNextLineFromFile(FILE * fin, bool & isEof)
{
	int ch;

	std::string line;
	do
	{
		ch = fgetc(fin);

		if (ch == '\r' || ch == '\n' || ch == EOF)
		{
			if (ch == '\r')
			{
				int nextCh = fgetc(fin);
				if (nextCh != '\n')
				{
					ungetc(nextCh,fin);
				}
			}
			break;
		}

		char buf[2];
		buf[0] = (char)ch;
		buf[1] = '\0';
		line.append(buf);

	} while(1);

	isEof = (ch == EOF);

	//line.append("\n");
	return line;
}


std::string LocalTimeAsString()
{
	char buf[2048];

#if _MSC_VER
	SYSTEMTIME currTime;
	GetLocalTime(&currTime);

	sprintf(buf,"%04d_%02d_%02d__%02d_%02d_%02d",
		(int)currTime.wYear,
		(int)currTime.wMonth,
		(int)currTime.wDay,
		(int)currTime.wHour,
		(int)currTime.wMinute,
		(int)currTime.wSecond);
#else
    time_t rawtime;
    time(&rawtime);

    struct tm local;
    localtime_r(&rawtime, &local);

	sprintf(buf,"%04d_%02d_%02d__%02d_%02d_%02d",
		(int)local.tm_year,
		(int)local.tm_mon,
		(int)local.tm_mday,
		(int)local.tm_hour,
		(int)local.tm_min,
		(int)local.tm_sec);
#endif

	return buf;
}

