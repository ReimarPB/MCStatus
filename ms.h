#ifdef WIN32
#include <windows.h>
typedef DWORD ms_t;
#else
typedef long long ms_t;
#endif

ms_t get_ms();

