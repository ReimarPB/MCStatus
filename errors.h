#include <stdbool.h>

#undef SOCKET_ERROR

extern bool error_colors;

enum error_type {
	INPUT_ERROR                = 0x00,
	SOCKET_ERROR               = 0x10,
	WINDOWS_ERROR              = 0x20,
	JAVA_PROTOCOL_ERROR        = 0x30,
	LEGACY_JAVA_PROTOCOL_ERROR = 0x40,
};

enum error {
	INPUT_ERROR_INVALID_EDITION_NAME      = 0x01,
	SOCKET_ERROR_CREATE_SOCKET_FAILED     = 0x11,
	SOCKET_ERROR_INVALID_HOSTNAME         = 0x12,
	SOCKET_ERROR_CONNECTION_FAILED        = 0x13,
	WINDOWS_ERROR_GET_CONSOLE_MODE_FAILED = 0x21,
	WINDOWS_ERROR_SET_CONSOLE_MODE_FAILED = 0x22,
	PROTOCOL_ERROR_INVALID_PACKET_ID      = 0x31,
	PROTOCOL_ERROR_INVALID_JSON           = 0x32,
	PROTOCOL_ERROR_INVALID_PONG           = 0x33,
};

bool error_is_type(enum error error, enum error_type type);

void error(enum error error, char* extra);

void assert_int(size_t actual, size_t expected, enum error error);

