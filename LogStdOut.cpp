#include "Log.h"
#include "Locks.h"
#include <iostream>

using namespace logging;
using namespace threading;
using namespace std;

static CMutex outstream_condition;

static const char *level2string(Level level)
{
    switch (level)
    {
        case DEBUG:
            return "DEBUG";
        case VERBOSE:
            return "VERBOSE";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

void logging::Write(Level level, const char *file, int line, const char *function, const string &msg)
{
	CScopedMutex m(outstream_condition);
	(level == ERROR ? cerr : cout) << level2string(level) << ": " << msg << endl;
}
