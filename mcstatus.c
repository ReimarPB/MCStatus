#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
#	pragma comment(lib, "Ws2_32.lib")
#	include <winsock2.h>
#	include <windows.h>
#	include "lib/wingetopt/src/getopt.h"
#else
#	include <unistd.h>
#	include <getopt.h>
#endif

#include "errors.h"
#include "server_status.h"
#include "protocols/protocols.h"

#define WHITE   "\x1b[0m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define RED     "\x1b[31m"
#define CYAN    "\x1b[36m"
#define GRAY    "\x1b[90m"

// TODO implement legacy java, implement bedrock

void print_usage(char* argv[])
{
	printf(
		"\nPrints the status of a Minecraft server\n\n"
		"Usage: %s <host>\n\n"
		"Arguments:\n"
		"    host  The hostname or IP address of the server\n"
        "          May include an optional port number\n\n"
		"Options:\n"
		"    -h, --help      Shows this help page\n"
		"    -e, --edition   Sets the Minecraft edition whose protocol it will use. Must be one of:\n"
		"                       java (default)  Minecraft Java Edition 1.7+\n"
		"                       legacy-java     Minecraft Java Edition 1.6-\n",
		argv[0]
	);
}

int main(int argc, char* argv[])
{
#ifdef _WIN32
	// Enable console colors
	HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD console_mode;
	if (GetConsoleMode(console_handle, &console_mode) == 0)
		system_error("Failed to enable console colors");
	
	if (SetConsoleMode(console_handle, console_mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) == 0)
		system_error("Failed to enable console colors");
#endif

	error_colors = true;

	char *edition = "java";

	struct option options[] = {
		{ "edition", required_argument, NULL, 'e' },
		{ "help",    no_argument,       NULL, 'h' },
		{ NULL,      0,                 NULL,  0  }
	};

	char opt;
	while ((opt = getopt_long(argc, argv, "he:", options, NULL)) != -1) {
		switch (opt) {
			case 'e':
				edition = optarg;
				break;
			case 'h':
				print_usage(argv);
				return 0;
			default:
				printf("Try '%s --help' for more information'", argv[0]);
				return 1;
		}
	}

	if (argc - optind < 1) {
		print_usage(argv);
		return 0;
	}

	// Parse hostname
	char *server = strtok(argv[optind], ":");
	char *port = strtok(NULL, ":");
	if (port == NULL) port = "25565";

	// Parse edition
	struct server_status status;
	if (strcmp(edition, "java") == 0)
		status = get_java_server_status(server, port);
	else if (strcmp(edition, "legacy-java") == 0)
		status = get_legacy_java_server_status(server, port);
	else
		error("Invalid edition name");

	// Print status
	printf(WHITE "\nServer status for " CYAN "%s\n", server);
	printf(GRAY  "~~~~~~~~~~~~~~~~~~");
	for (int i = 0; i < strlen(server); ++i) putchar('~');
	putchar('\n');
	if (status.version_name && status.protocol_version) printf(WHITE "Version: " YELLOW "%s" GRAY " (protocol version %d)\n", status.version_name, status.protocol_version);
	printf(WHITE "Players: " YELLOW "%d" WHITE " / " YELLOW "%d\n", status.online_players, status.max_players);

	const char* ping_color = GREEN;
	if (status.ping > 300) ping_color = YELLOW;
	if (status.ping > 600) ping_color = RED;
	printf(WHITE "Ping: %s" MS "\n\n", ping_color, status.ping);

	if (status.motd) printf("\n" WHITE "%s\n\n", status.motd);
	printf(WHITE);

	// Clean up
	if (status.version_name) free(status.version_name);
	if (status.motd) free(status.motd);

	return EXIT_SUCCESS;
}
