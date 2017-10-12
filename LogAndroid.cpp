#include "Log.h"
#include <android//log.h>

using namespace std;
using namespace logging;
  
static int level2android(Level level)
{
	switch (level)
	{
        case DEBUG:
            return ANDROID_LOG_DEBUG;
        case VERBOSE:
            return ANDROID_LOG_VERBOSE;
        case INFO:
            return ANDROID_LOG_INFO;
        case WARNING:
            return ANDROID_LOG_WARN;
        case ERROR:
            return ANDROID_LOG_ERROR;
        default:
            return 0;
	}
}

void logging::Write(Level level, const char *file, int line, const char *function, const string &message)
{
	__android_log_print(level2android(level), "drunken-ninja", message.c_str());
}
