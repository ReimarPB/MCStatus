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

bool error_is_type(enum error error, enum error_type error_type)
{
	return error > error_type && error < error_type + 0x10;
}

void error(enum error error, char* extra)
{
	if (error_colors) printf("\x1b[91m");
	printf("ERROR: ");	

	// Get error message
	char *message;
	switch (error) {
		case INPUT_ERROR_INVALID_EDITION_NAME:      message = "Invalid edition name";                       break;
		case SOCKET_ERROR_CREATE_SOCKET_FAILED:     message = "Failed to create socket";                    break;
		case SOCKET_ERROR_INVALID_HOSTNAME:         message = "Invalid hostname";                           break;
		case SOCKET_ERROR_CONNECTION_FAILED:        message = "Failed to connect to server";                break;
		case WINDOWS_ERROR_GET_CONSOLE_MODE_FAILED: message = "Failed to get console mode";                 break;
		case WINDOWS_ERROR_SET_CONSOLE_MODE_FAILED: message = "Failed to set console mode";                 break;
		case PROTOCOL_ERROR_INVALID_PACKET_ID:      message = "Invalid packet ID received from server";     break;
		case PROTOCOL_ERROR_INVALID_JSON:           message = "Invalid JSON received from server";          break;
		case PROTOCOL_ERROR_INVALID_PONG:           message = "Invalid pong response received from server"; break;
	}
	printf("%s\n", message);
	if (extra) printf("%s\n", extra);

	// Show error code
#ifdef _WIN32
	if (error_is_type(error, SOCKET_ERROR))
		printf("Error code: %d\n", WSAGetLastError());
	else if (error_is_type(error, WINDOWS_ERROR))
		printf("Error code: %d\n", GetLastError());
#else
	if (error_is_type(error, SOCKET_ERROR))
		printf("Error code: %d\n", errno);
#endif

	if (error_colors) printf("\x1b[0m");
	exit(error);
}

void assert_int(size_t actual, size_t expected, enum error err)
{
	if (actual == expected) return;

	char *message = malloc(32);
	snprintf(message, 32, "Expected %zd, got %zd", expected, actual);
	error(err, message);
}

