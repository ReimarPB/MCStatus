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

// Names of protocol versions
// https://minecraft.fandom.com/wiki/Protocol_version
char *versions[] = {
	"Alpha v1.0.17 - Alpha v1.0.17_04",
	"Alpha v1.1.0 - Alpha v1.1.2_01",
	"Alpha v1.2.0 - Alpha v1.2.1_01",
	"Alpha v1.2.2",
	"Alpha v1.2.3 - Alpha v1.2.3_04",
	"Alpha v1.2.3_05 - Alpha v1.2.6",
	"Beta 1.0 - Beta 1.1_01",
	"Beta 1.1_02 - Beta 1.2_02",
	"Beta 1.3 - Beta 1.3_01",
	"Beta 1.4 - Beta 1.4_01",
	"Beta 1.5 - Beta 1.5_02",
	"Beta 1.6 Test Build 3",
	"Beta 1.6 - Beta 1.6.6",
	"Beta 1.7 - Beta 1.7.3",
	"Beta 1.8 Pre-release",
	"Beta 1.8 Pre-release 2 ;)",
	"Beta 1.8 - Beta 1.8.1",
	"Beta 1.9 Prerelease",
	"Beta 1.9 Prerelease 2 - Beta 1.9 Prerelease 3",
	"Beta 1.9 Prerelease 4",
	"Beta 1.9 Prerelease 5",
	"1.0.x",
	"1.1",
	"12w03a - 12w05b",
	"12w06a",
	"12w07a - 12w07b",
	"1.2 - 1.2.3",
	"1.2.4 - 1.2.5",
	"12w16a",
	"12w17a",
	"12w18a - 12w19a",
	"12w21a - 12w21b",
	"12w22a",
	"12w23a - 12w23b",
	"12w24a",
	"12w25a - 12w26a",
	"12w27a - 12w30b",
	"1.3.x",
	"12w32a",
	"12w34a",
	"12w34b - 12w37a",
	"12w38a - 12w39w",
	"12w40a",
	"12w40b",
	"12w41a - 12w42a",
	"1.4 - 1.4.2",
	"1.4.3",
	"1.4.4 - 1.4.5",
	"12w49a",
	"1.4.6 - 1.4.7",
	"13w01a - 13w01b",
	"13w02a - 13w02b",
	"13w03a",
	"13w04a",
	"13w05a - 13w05b",
	"13w06a - 13w07a",
	"13w09a - 13w09b",
	"1.5 - 1.5.1",
	"1.5.2",
	"13w16a",
	"13w16b",
	"13w17a",
	"13w18a - 13w18c",
	"13w19a",
	"13w21a - 13w23a",
	"13w23b",
	"13w24a",
	"13w24b",
	"13w25a - 13w25c",
	"1.6",
	"1.6.1",
	"1.6.2",
	"13w36a - 13w26b",
	"13w37a - 13w37b",
	"1.6.3",
	"1.6.4",
	"13w38a - 13w38c",
	"13w39a - 13w39b"
};

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

	if (strncmp(data, "§1\0", 3) == 0) {
		// TODO implement
	} else {
		// TODO add error handling
		status.motd = strtok(string, "§");
		status.online_players = atoi(strtok(NULL, "§"));
		status.max_players = atoi(strtok(NULL, "§"));
	}

	status.ping = get_ms() - ping_time;

	free(data);

	return status;
} 

