#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "LogToFile.h"

#define DEBUG	1

#ifdef	DEBUG
static	int		defaultIgnoreMsgs = 0;
#else
static	int		defaultIgnoreMsgs = 1;
#endif

LogToFile::LogToFile (int unlinkFirst)
{
	// Is this Linux or Windows

#ifdef	WIN32
	// They will always look for the file in the C: root dir
	strcpy (myFileName, "C:/XPertMUD.log");
#else
	// Look for the file where they started the program
	strcpy (myFileName, "XPertMUD.log");
#endif

	if (unlinkFirst)
		unlink (myFileName);

	myIgnoreMsgs = defaultIgnoreMsgs;
}

LogToFile::LogToFile (char  *fname, int unlinkFirst)
{
	strcpy (myFileName, fname);
	if (unlinkFirst)
		unlink (myFileName);

	myIgnoreMsgs = defaultIgnoreMsgs;
}

LogToFile::~LogToFile ()
{
}

void LogToFile::setIgnoreMsgs (
	int ignoreMsgs)
{
	myIgnoreMsgs = ignoreMsgs;
}

int  LogToFile::getIgnoreMsgs ()
{
	return (myIgnoreMsgs);
}

void LogToFile::logMsg (
	char	*msg,		// message to log
	int		lf)			// emit a lf after
{
	if (myIgnoreMsgs)
		return;

	FILE	*file;

	file = fopen (myFileName, "ab");
	if (file == 0)
		return;

	fprintf (file, "%s", msg);
	if (lf)
		fprintf (file, "\n");

	fclose (file);
}

void LogToFile::logBuffer (
	char	*buffer,
	int		num)
{
	if (myIgnoreMsgs)
		return;

	int		i, j, k, cc;
	char	out [128];

	for (i = 0; i < num; i += 16)
	{
		out [0] = 0;

		for (j = 0; j < 16; j ++)
		{
			k = i + j;
			if (k >= num)
			{
				sprintf (&out [strlen (out)], "   ");
			}
			else
			{
				cc = (buffer [k] & 0x000000FF);
				sprintf (&out [strlen (out)], "%02x ", cc);
			}
		}

		sprintf (&out [strlen (out)], " - ");

		for (j = 0; j < 16; j ++)
		{
			k = i + j;
			if (k >= num)
			{
				sprintf (&out [strlen (out)], " ");
			}
			else
			{
				cc = (buffer [k] & 0x000000FF);
				if (cc < ' ' || cc > '~')
					cc = '.';
				sprintf (&out [strlen (out)], "%c", cc);
			}
		}

		logMsg (out, 1);
	}
}

void LogToFile::logMsgF(char *lpOutputString, ...)
{
    char achBuffer[2048];

    /* create the output buffer */
    va_list args;
    va_start(args, lpOutputString);
    vsprintf(achBuffer, lpOutputString, args);
    va_end(args);

	logMsg (achBuffer, 0);
}

