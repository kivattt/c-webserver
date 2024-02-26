#ifndef SERVER_H
#define SERVER_H

#include <sys/socket.h>
#include <arpa/inet.h>

int start_server(const char* listenAddr, const unsigned short port);
void dispatch_connection(int socket, const char* listenAddr, const unsigned short port);
void *handle_connection(void *args);

#endif // SERVER_H
