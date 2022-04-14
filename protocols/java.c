#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#	include <winsock2.h>
#else
#	include <arpa/inet.h>
// TODO find the right imports
#endif

#include "../lib/cJSON/cJSON.h"
#include "../errors.h"
#include "mcstatus_result.h"
#include "chat_parser.h"
#include "tcp.h"

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
struct mcstatus_result get_java_server_status(char *server, char *port)
{
	int sock = tcp_connect(server, port);

	// Send server status request	
	{
		uint8_t packet_length = 10 + strlen(server);
		uint8_t packet_id = 0x00;
		uint8_t protocol_version[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0x0F }; // -1 as VarInt
		uint8_t server_len = strlen(server);
		uint16_t port = htons(25565); // TODO
		uint8_t next_state = 0x01; // Server status

		send(sock, &packet_length,    1,                        0);
		send(sock, &packet_id,        1,                        0);
		send(sock, &protocol_version, sizeof(protocol_version), 0);
		send(sock, &server_len,       1,                        0);
		send(sock, server,            server_len,               0);
		send(sock, &port,             2,                        0);
		send(sock, &next_state,       1,                        0);
	}
	{
		uint8_t packet_length = 1;
		uint8_t packet_id = 0x00;

		send(sock, &packet_length, 1, 0);
		send(sock, &packet_id,     1, 0);
	}

	// Receive response
	int packet_length = read_varint(sock);

	uint8_t packet_type;
	recv(sock, &packet_type, 1, 0);

	if (packet_type != 0x00)
		error("Invalid packet type received from server", 3);

	int string_length = read_varint(sock);
	
	uint8_t *data = malloc(string_length);
	recv(sock, data, string_length, MSG_WAITALL);

	// Parse response
	cJSON *json = cJSON_ParseWithLength(data, string_length);
	if (cJSON_IsInvalid(json))
		error("Invalid JSON received from server", 3);

	struct mcstatus_result result;

	cJSON *version = cJSON_GetObjectItemCaseSensitive(json, "version");
	if (cJSON_IsObject(version)) {

		cJSON *version_name = cJSON_GetObjectItemCaseSensitive(version, "name");
		if (cJSON_IsString(version_name))
			result.version_name = chat_string_to_ansi_string(version_name->valuestring);

		cJSON *protocol_version = cJSON_GetObjectItemCaseSensitive(version, "protocol");
		if (cJSON_IsNumber(protocol_version))
			result.protocol_version = protocol_version->valueint;

	}
	cJSON *players = cJSON_GetObjectItemCaseSensitive(json, "players");
	if (cJSON_IsObject(players)) {

		cJSON *online_players = cJSON_GetObjectItemCaseSensitive(players, "online");
		if (cJSON_IsNumber(online_players))
			result.online_players = online_players->valueint;
		
		cJSON *max_players = cJSON_GetObjectItemCaseSensitive(players, "max");
		if (cJSON_IsNumber(max_players))
			result.max_players = max_players->valueint;
		
		// TODO sample players
	}
	cJSON* description = cJSON_GetObjectItemCaseSensitive(json, "description");
	if (cJSON_IsObject(description)) {
		result.motd = chat_object_to_ansi_string(description);
	} else if (cJSON_IsString(description)) {
		result.motd = chat_string_to_ansi_string(description->valuestring);	
	}

	cJSON_Delete(json);
	free(data);

	tcp_disconnect(sock);

	return result;
}
