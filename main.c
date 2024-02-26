#include <stdio.h>
#include <sys/socket.h>

#include "server.c"

int main(){
	start_server("127.0.0.1", 1337);
}
