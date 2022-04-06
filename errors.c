#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

// TODO add specific error codes

// Print generic prefix/suffix for error messages
void begin_error()
{
	printf("\x1b[91mERROR: ");	
}

void end_error()
{
	printf("\x1b[0m");
}

// Generic error handler with custom message and exit code
int error(char *message, int code)
{
	begin_error();
	printf("%s", message);
	end_error();
	exit(code);
}

enum socket_error {
	SOCKET_ERROR_CREATE_SOCKET_FAILED = 0x01,
	SOCKET_ERROR_INVALID_HOSTNAME     = 0x02,
	SOCKET_ERROR_CONNECTION_FAILED    = 0x03,
};

// Error in sockets, should use WSAGetLastError on Windows
int socket_error(char *message/*enum socket_error*/)
{
	begin_error();

	/*char *message;
	switch (socket_error) {
		case SOCKET_ERROR_CREATE_SOCKET_FAILED: message = "Failed to create socket";     break;
		case SOCKET_ERROR_INVALID_HOSTNAME:     message = "Invalid hostname";            break;
		case SOCKET_ERROR_CONNECTION_FAILED:    message = "Failed to connect to server"; break;
	}*/
	printf("%s\n", message);

#ifdef _WIN32
	printf("Error code: %d\n", WSAGetLastError());
#endif

	end_error();
	exit(1);
}

// Error while setting the console mode on Windows
// Should not display colors, and should use GetLastError
#ifdef _WIN32
int console_mode_error(char *message)
{
	printf("ERROR: %s\nError code: %d\n", message, GetLastError());
	return 2;
}
#endif

