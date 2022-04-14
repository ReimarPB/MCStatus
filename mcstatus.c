#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#	pragma comment(lib, "Ws2_32.lib")
#	include <winsock2.h>
#	include <windows.h>
//#	include "lib/wingetopt/getopt.h";
#else
#	include <unistd.h>
#	include <getopt.h>
#endif

#include "errors.h"
#include "protocols/protocols.h"
#include "protocols/mcstatus_result.h"

#define WHITE   "\x1b[0m"
#define YELLOW  "\x1b[33m"
#define CYAN    "\x1b[36m"
#define GRAY    "\x1b[90m"

// TODO add options, implement legacy java, implement bedrock

int main(int argc, char* argv[])
{
	if (argc < 2) {
		printf("Too few arguments");
		return 0;
	}

#ifdef _WIN32
	// Enable console colors
	HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD console_mode;
	if (GetConsoleMode(console_handle, &console_mode) == 0)
		return console_mode_error("Failed to get console mode");		
	
	if (SetConsoleMode(console_handle, console_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) == 0)
		return console_mode_error("Failed to change console mode");
#endif

	char *server = strtok(argv[1], ":");
	char *port = strtok(NULL, ":");
	if (port == NULL) port = "25565";

	char *edition = "java"; // TODO
	
	struct mcstatus_result result;
	if (strcmp(edition, "java") == 0)
		result = get_java_server_status(server, port);
	else if (strcmp(edition, "legacy-java") == 0)
		result = get_legacy_java_server_status(server, port);
	else
		error("Invalid edition name", 69); // TODO

	// Print result
	printf(WHITE "\nServer status for " CYAN "%s\n", server);
	printf(GRAY  "~~~~~~~~~~~~~~~~~~");
	for (int i = 0; i < strlen(server); ++i) putchar('~');
	putchar('\n');
	printf(WHITE "Version: " YELLOW "%s" GRAY " (protocol version %d)\n", result.version_name, result.protocol_version);
	printf(WHITE "Players: " YELLOW "%d" WHITE " / " YELLOW "%d\n\n", result.online_players, result.max_players);
	if (result.motd) printf(WHITE "%s\n", result.motd);
	printf(WHITE);

	// Clean up
	free(result.version);
	if (result.motd) free(result.motd);

	return 0;
}
