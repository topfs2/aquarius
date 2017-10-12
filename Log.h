#pragma once

#include <string>
#include <sstream>
#include <stdexcept>

namespace logging
{
	#define LOG(level, msg)                                                    \
	({                                                                         \
    	std::ostringstream os;                                                 \
		os << msg;                                                             \
    	logging::Write(level, __FILE__, __LINE__, __FUNCTION__, os.str());     \
	})

	#define LOG_AND_THROW(level, msg)                                          \
	({                                                                         \
    	std::ostringstream os;                                                 \
		os << msg;                                                             \
    	logging::Write(level, __FILE__, __LINE__, __FUNCTION__, os.str());     \
		throw std::runtime_error(os.str());                                    \
	})

//#ifndef NDEBUG
	#define DLOG(level, msg)                                                   \
	({                                                                         \
		LOG(level, msg);                                                       \
	})
//#else
//	#define DLOG(level, msg) ()
//#endif

    enum Level
    {
        DEBUG = 0,
        VERBOSE,
        INFO,
        WARNING,
        ERROR
    };

	// This is the LOG specific implementation, it must be threadsafe
    extern void Write(Level level, const char *file, int line, const char *function, const std::string &message);
};
