#ifndef     _LOGTOFILE_H_
#define     _LOGTOFILE_H_

#include <stdarg.h>

class LogToFile 
{
public:

	LogToFile (int unlinkFirst);
	LogToFile (char   *fname, int unlinkFirst);

	virtual ~LogToFile ();

	void logMsg (
		char	*msg,		// message to log
		int		lf);		// emit a lf after

	void logBuffer (
		char	*buffer,
		int		num);

	void logMsgF (char *lpOutputString, ...);

	void setIgnoreMsgs (int ignoreMsgs);
	int  getIgnoreMsgs ();

private:

	char    myFileName [512];
	int		myIgnoreMsgs;
};

extern 	LogToFile	*myLog;

#endif   /* _LOGTOFILE_H_ */

