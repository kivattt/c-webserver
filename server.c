#ifndef SERVER_C
#define SERVER_C

#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#include "server.h"
//#include "http.h"
#include "http.c"

struct our_connection {
	int socket;
	struct sockaddr_in address;
};

int start_server(const char* listenAddr, const unsigned short port) {
	int s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		fprintf(stderr, "Failed to create socket\n");
		return -1;
	}

	if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) != 0) {
		fprintf(stderr, "Failed to set socket options\n");
		return -2;
	}

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(listenAddr);
	address.sin_port = htons(port);

	if (bind(s, (struct sockaddr*) &address, sizeof(address)) != 0) {
		fprintf(stderr, "Failed to bind to port: %d\n", port);
		return -3;
	}

	if (listen(s, 128) != 0) {
		fprintf(stderr, "Failed to start listening\n");
		return -4;
	}

	printf("Server listening on: %s:%d\n", listenAddr, port);

	for (;;)
		dispatch_connection(s, listenAddr, port);

	return 0;
}

void dispatch_connection(int socket, const char* listenAddr, const unsigned short port) {
	struct sockaddr_in address;
	socklen_t addlen = sizeof(address);

	const int s = accept(socket, (struct sockaddr *) &address, &addlen);
	if (s < 0) {
		fprintf(stderr, "Error on accepting connection\n");
		return;
	}
	
//	const struct timeval timeout = {5, 0};
	const struct timeval timeout = {0, 500};

	if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) != 0) 
		fprintf(stderr, "Failed to set a timeout\n");

	if (setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) != 0) 
		fprintf(stderr, "Failed to set a timeout\n");

	struct our_connection *c = malloc(sizeof(*c));
	c->socket = s;
	c->address = address;

	pthread_t id;
	if (pthread_create(&id, NULL, &handle_connection, c) != 0) {
		fprintf(stderr, "Couldn't spawn a thread");
		return;
	}

	pthread_detach(id);
}

void *handle_connection(void *args) {
	struct our_connection *c = (struct our_connection *) args;

	const char *ip = inet_ntoa(c->address.sin_addr);
	printf("Incoming connection from: %s\n", ip);

	uint8_t buffer[2048];
	memset(buffer, 0, 2048);

	const int r = recv(c->socket, buffer, sizeof(buffer), MSG_WAITALL);
	if (r <= 0) {
		fprintf(stderr, "No data received from client (%s)\n", strerror(errno));
		close(c->socket);
		free(c);
		pthread_exit(NULL);
		return 0;
	}

	struct HTTPRequest request = get_parsed_request(buffer);
	printf("Method found: %s\n", request.method);

//	printf("Got data: %s\n", buffer);

	write(c->socket, "hello", 5);
	close(c->socket);
	free(c);
	pthread_exit(NULL);

	return NULL;
}

#endif // SERVER_C
