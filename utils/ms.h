#ifdef WIN32
#	include <windows.h>
#	define MS "%d"
typedef DWORD ms_t;
#else
#	define MS "%lld"
typedef long long ms_t;
#endif

ms_t get_ms();

