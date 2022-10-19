#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    int sd;
    struct sockaddr_in server;
    struct hostent *hp;

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    hp = gethostbyname(argv[1]);
    bcopy(hp->h_addr, &(server.sin_addr), hp->h_length);

    for(;;) {
        sendto(sd, "HI", 2, 0, (struct sockaddr *) &server, sizeof(server));
        sleep(2);

    }
    close(sd);

}