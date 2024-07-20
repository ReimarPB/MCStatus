#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cjson/cJSON.h>

#ifdef _WIN32
#	include <winsock2.h>
#else
#	include <arpa/inet.h>
#endif

#include "../errors.h"
#include "../server_status.h"
#include "../utils/chat_parser.h"
#include "../utils/tcp.h"
#include "../utils/ms.h"

// https://wiki.vg/Protocol#VarInt_and_VarLong
int read_varint(int sock)
{
	int value = 0;
	int length = 0;
	uint8_t byte; 
	while (1) {
		recv(sock, &byte, 1, 0);
		value |= (byte & 0x7F) << (length * 7);
		length++;
		if ((byte & 0x80) != 0x80) break;
	}
	return value;
}

// Gets the server status of a modern Java server
// See https://wiki.vg/Server_List_Ping
struct server_status get_java_server_status(char *server, char *port)
{
	int sock = tcp_connect(server, port);

	// Send server status request	
	{
		uint8_t packet_id = 0x00;
		uint8_t protocol_version[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0x0F }; // -1 as VarInt
		uint8_t server_len = strlen(server);
		uint16_t port_num = htons(atoi(port));
		uint8_t next_state = 0x01; // Server status
		uint8_t packet_length = sizeof(packet_id) + sizeof(protocol_version) + sizeof(server_len) + server_len + sizeof(port_num) + sizeof(next_state);

		send(sock, &packet_length,    sizeof(packet_length),    0);
		send(sock, &packet_id,        sizeof(packet_id),        0);
		send(sock, &protocol_version, sizeof(protocol_version), 0);
		send(sock, &server_len,       sizeof(server_len),       0);
		send(sock, server,            server_len,               0);
		send(sock, &port_num,         sizeof(port_num),         0);
		send(sock, &next_state,       sizeof(next_state),       0);
	}
	{
		uint8_t packet_length = 1;
		uint8_t packet_id = 0x00;

		send(sock, &packet_length, 1, 0);
		send(sock, &packet_id,     1, 0);
	}

	// Receive response
	{
		int packet_length = read_varint(sock);

		uint8_t packet_id;
		recv(sock, &packet_id, sizeof(packet_id), 0);

		assert_int(packet_id, 0x00, "Received invalid packet ID from server");
	}

	int string_length = read_varint(sock);
	
	uint8_t *data = malloc(string_length);
	recv(sock, data, string_length, MSG_WAITALL);
	data[string_length] = '\0';

	// Parse response
	cJSON *json = cJSON_ParseWithLength(data, string_length);
	if (cJSON_IsInvalid(json))
		error("Received invalid JSON from server");

	struct server_status status;

	status.json = data;

	cJSON *version = cJSON_GetObjectItemCaseSensitive(json, "version");
	if (cJSON_IsObject(version)) {

		cJSON *version_name = cJSON_GetObjectItemCaseSensitive(version, "name");
		if (cJSON_IsString(version_name))
			status.version_name = chat_string_to_ansi_string(version_name->valuestring);

		cJSON *protocol_version = cJSON_GetObjectItemCaseSensitive(version, "protocol");
		if (cJSON_IsNumber(protocol_version))
			status.protocol_version = protocol_version->valueint;

	}
	cJSON *players = cJSON_GetObjectItemCaseSensitive(json, "players");
	if (cJSON_IsObject(players)) {

		cJSON *online_players = cJSON_GetObjectItemCaseSensitive(players, "online");
		if (cJSON_IsNumber(online_players))
			status.online_players = online_players->valueint;
		
		cJSON *max_players = cJSON_GetObjectItemCaseSensitive(players, "max");
		if (cJSON_IsNumber(max_players))
			status.max_players = max_players->valueint;
		
		// TODO sample players
	}
	cJSON* description = cJSON_GetObjectItemCaseSensitive(json, "description");
	if (cJSON_IsObject(description)) {
		status.motd = chat_object_to_ansi_string(description);
	} else if (cJSON_IsString(description)) {
		status.motd = chat_string_to_ansi_string(description->valuestring);
	}

	cJSON_Delete(json);

	ms_t ping_time = get_ms();

	// Send ping
	uint8_t packet_id = 0x01;
	int64_t payload = 0;
	uint8_t packet_length = sizeof(packet_id) + sizeof(payload);

	send(sock, &packet_length, sizeof(packet_length), 0);
	send(sock, &packet_id,     sizeof(packet_id),     0);
	send(sock, &payload,       sizeof(payload),       0);

	// Receive pong
	int response_packet_length = read_varint(sock);

	uint8_t response_packet_id;
	recv(sock, &response_packet_id, sizeof(response_packet_id), 0);
	assert_int(response_packet_id, 0x01, "Received invalid packet ID for pong response from server");

	uint64_t response;
	recv(sock, &response, sizeof(response), 0);
	assert_int(response, payload, "Received invalid pong payload from server");

	// Calculate time difference
	status.ping = get_ms() - ping_time;

	tcp_disconnect(sock);

	return status;
}
