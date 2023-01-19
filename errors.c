#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <errno.h>
#endif

#include "errors.h"

bool json;
bool error_colors = false;

void begin_error()
{
	if (json) fprintf(stderr, "{\"error\":\"");
	else if (error_colors) fprintf(stderr, "\x1b[31m");
}

void end_error()
{
	if (json) fprintf(stderr, "\"}");
	else if (error_colors) fprintf(stderr, "\x1b[0m");
	putchar('\n');
}

void error_with_code(char *message, int code)
{
	begin_error();

	if (code) {
#ifdef _WIN32
		char *error_message = NULL;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&error_message, 0, NULL);
		fprintf(stderr, "%s: %s", message, error_message);
		LocalFree(error_message);
#else
		perror(message);
#endif
	} else fprintf(stderr, "%s", message);

	end_error();

	if (code) exit(code);
	else exit(EXIT_FAILURE);
}

void try(int result, char* message)
{
	if (result != 0) error_with_code(message, result);
}

void socket_error(char *message)
{
#ifdef _WIN32
	int code = WSAGetLastError();
#else
	int code = errno;
#endif
	error_with_code(message, code);
}

void system_error(char *message)
{
#ifdef _WIN32
	int code = GetLastError();
#else
	int code = errno;
#endif
	error_with_code(message, code);
}

void error(char *message)
{
	begin_error();
	fprintf(stderr, "%s", message);
	end_error();
	exit(EXIT_FAILURE);
}

void assert_int(size_t actual, size_t expected, char* error)
{
	if (actual == expected) return;

	begin_error();
	fprintf(stderr, "%s: Expected %zd, got %zd", error, expected, actual);
	end_error();
	exit(EXIT_FAILURE);
}

