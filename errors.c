#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef _WIN32
#	include <windows.h>
#else
#	include <errno.h>
#endif

#include "errors.h"

bool error_colors = false;

void show_color()
{
	if (error_colors) fprintf(stderr, "\x1b[31m");
}

void reset_color()
{
	if (error_colors) fprintf(stderr, "\x1b[0m");
}

void error_with_code(char *message, int code)
{
	show_color();


	if (code) {
#ifdef _WIN32
		char *error_message = NULL;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&error_message, 0, NULL);
		fprintf(stderr, "%s: %s", message, error_message);
		LocalFree(error_message);
#else
		perror(message);
#endif
	} else puts(message);

	reset_color();

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
	show_color();
	puts(message);
	reset_color();
	exit(EXIT_FAILURE);
}

void assert_int(size_t actual, size_t expected, char* error)
{
	if (actual == expected) return;

	show_color();
	fprintf(stderr, "%s: Expected %zd, got %zd\n", error, expected, actual);
	reset_color();
	exit(EXIT_FAILURE);
}

