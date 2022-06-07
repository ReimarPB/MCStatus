#include "../ms.h"

#ifndef MCSTATUS_RESULT_H
#define MCSTATUS_RESULT_H

struct mcstatus_result {
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

