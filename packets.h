struct JavaHandshakePacket {
	int packet_type;
	int protocol_version;
	char *server_address;
	unsigned short port;
	int next_state;
};

