/*#include <stdint.h>

#ifdef __unix__
#include <arpa/inet.h>
#elif defined _WIN32
#include <winsock2.h>
#endif 

struct Packet {
	unsigned int size;
	uint8_t *data;
};

void grow_packet(struct Packet *packet, unsigned int size)
{
	printf("Growing packet of size %d with %d so the size becomes %d\n", packet->size, size, packet->size + size);
	printf("Address of existing data: %d\n", packet->data);
	uint8_t *new_data;
	if (packet->size == 0)
		new_data = malloc(size);
	else
		new_data = realloc(*(packet->data), packet->size + size);
	printf("Success, new address: %d\n", new_data);

	// TODO use memcpy here as well

	packet->size += size;
	packet->data = new_data;	
}

void write_varint(int sock, signed int value)
{
	while (1) {
		if ((value & ~0x7F) == 0) {
			write(sock, value, 1);
			return;
		}

		write(sock, (value & 0x7F) | 0x80);
		value >>= 7;
	} 
}

void send_java_packet(int sock, unsigned char *packet)
{
	write_varint(sock, sizeof(*packet));
	write(sock, packet, sizeof(*packet));
}

void add_varint(struct Packet *packet, signed int value)
{
	while (1) {
		if ((value & ~0x7F) == 0) {
			grow_packet(packet, 1);
			memcpy(&packet->data + packet->size - 1, &value, 1);
			return;
		}

		grow_packet(packet, 1);
		int8_t value_to_add = (value & 0x7F) | 0x80;
		memcpy(&packet->data + packet->size - 1, &value_to_add, 1);
		value >>= 7;
	} 
}

void add_string(struct Packet *packet, char *value)
{
	printf("Length of string: %d\n", sizeof(value));
	add_varint(packet, sizeof(value));
	grow_packet(packet, sizeof(value));
	memcpy(&packet->data + packet->size - sizeof(value), &value, sizeof(value)); 
}

void add_ushort(struct Packet *packet, uint16_t value)
{
	grow_packet(packet, 2);
	uint16_t be_value = htons(value);
	memcpy(&packet->data + packet->size - 2, &be_value, 2);
}*/
