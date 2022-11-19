#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <uchar.h>
#include <string.h>

#ifdef _WIN32
#	include <winsock2.h>
#else
#	include <arpa/inet.h>
#endif

#include "../errors.h"
#include "../server_status.h"
#include "../utils/tcp.h"
#include "../utils/text_converter.h"

// Gets the server status of a legacy Java Edition server (1.6 and below)
// https://wiki.vg/Server_List_Ping#1.6
struct server_status get_legacy_java_server_status(char *server, char *port)
{
	ms_t ping_time = get_ms();

	// Send request
	int sock = tcp_connect(server, port);
	uint8_t packet[] = { 0xFE, 0x01, 0xFA };
	send(sock, &packet, sizeof(packet), 0);

	// Get response
	uint8_t packet_id;
	recv(sock, &packet_id, 1, 0);
	assert_int(packet_id, 0xFF, "Received invalid packet ID from server");

	uint16_t packet_length;
	recv(sock, &packet_length, 2, MSG_WAITALL);
	size_t string_length = ntohs(packet_length) * 2;

	unsigned char *data = malloc(string_length);
	recv(sock, data, string_length, MSG_WAITALL);

	char *string = utf16be_to_utf8(data, string_length);
	struct server_status status;
	memset(&status, 0, sizeof(status));

	if (strncmp(string, "§1\0", 4) == 0) {

		int index = 3;
		for (int i = 0; i < 5; i++) {
			free(string);
			string = utf16be_to_utf8(data + index * 2, string_length - index * 2);

			index += strlen(string) + 1;

			int result = -1;
			switch (i) {
				case 0:
					result = sscanf(string, "%d", &status.protocol_version);
					break;
				case 1:
					status.version_name = strdup(string);
					break;
				case 2:
					status.motd = strdup(string);
					break;
				case 3:
					result = sscanf(string, "%d", &status.online_players);
					break;
				case 4:
					result = sscanf(string, "%d", &status.max_players);
					break;
			}

			if (result == 0) error("Server response is in an invalid format");
		}
		free(string);

	} else {

		char *motd = malloc(string_length - 4);
		int online_players, max_players;

		int result = sscanf(string, "%[^§]§%d§%d", motd, &online_players, &max_players);
		if (result < 3)	error("Server response is in an invalid format");

		status.motd = motd;
		status.online_players = online_players;
		status.max_players = max_players;

	}

	status.ping = get_ms() - ping_time;

	tcp_disconnect(sock);
	free(data);

	return status;
} 

