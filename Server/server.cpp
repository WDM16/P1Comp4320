#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    int n, sd;
    struct sockaddr_in server;
    char buf[512];

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(12345);

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    bind(sd, (struct sockaddr *)&server, sizeof(server));
    
    for(;;) {
        n = recv (sd, buf, sizeof(buf), sizeof(server));
        buf[n] = '\0';
        printf("Received: %s\n", buf);

    }

    close(sd);
    return 0;
}