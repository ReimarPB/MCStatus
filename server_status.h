#include "utils/ms.h"

#ifndef SERVER_STATUS_H
#define SERVER_STATUS_H

struct server_status {
	char *version_name;
	int protocol_version;
	int online_players;
	int max_players;
	// TODO player sample
	char* motd;
	char* favicon;
	// TODO mod list
	ms_t ping;
};

#endif

