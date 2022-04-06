void begin_error();

void end_error();

void error(char *message, int code);

void socket_error(char *message);

#ifdef _WIN32
int windows_error(char *message);
#endif

