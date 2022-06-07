#ifndef WIN32
#	include <stddef.h>
#	include <sys/time.h>
#endif

#include "ms.h"

ms_t get_ms()
{
#ifdef WIN32
	return GetTickCount();
#else
	struct timeval time;
	gettimeofday(&time, NULL);
	return time.tv_sec * 1000LL + time.tv_usec / 1000LL;
#endif
}

