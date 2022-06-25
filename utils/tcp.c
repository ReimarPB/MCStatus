#include <string.h>

#ifdef _WIN32
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	include <windows.h>
#else
#	include <errno.h>
#	include <netdb.h>
#	include <sys/types.h>
#	include <arpa/inet.h>
#	include <unistd.h>
#endif

#include "../errors.h"

int tcp_connect(char *server, char *port)
{
#ifdef _WIN32
	// Startup socket
	WSADATA wsa_data;
	WSAStartup(MAKEWORD(2,2), &wsa_data);
#endif

	// Create socket
	int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1)
		socket_error("Failed to create socket");
	
	// Get IP address from hostname
	struct addrinfo hints, *addr;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(server, port, &hints, &addr) != 0)
		socket_error("Invalid hostname");
	
	// Connect to server	
	if (connect(sock, addr->ai_addr, addr->ai_addrlen) != 0)
		socket_error("Unable to connect to server");

	freeaddrinfo(addr);

	return sock;
}

void tcp_disconnect(int sock)
{
#ifdef _WIN32
	closesocket(sock);
	WSACleanup();	
#else
	close(sock);
#endif
}

