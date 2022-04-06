#include <stdint.h>

struct Packet {
	unsigned int size;
	unsigned char *data;
};

void grow_packet(struct Packet *packet, unsigned int size);

void write_varint(int sock, int value);

void send_java_packet(int sock, uint8_t *packet);

void add_varint(struct Packet *packet, int value);

void add_string(struct Packet *packet, char *value);

void add_ushort(struct Packet *packet, unsigned short value);
