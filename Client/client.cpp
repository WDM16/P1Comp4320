#include <fstream>
#include <iostream>
#include <vector>
#include <cstdint>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>

int main(int argc, char **argv) {
    int sd;
    struct sockaddr_in server;
    struct hostent *hp;

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    hp = gethostbyname("172.19.68.164");
    bcopy(hp->h_addr, &(server.sin_addr), hp->h_length);

    for(;;) {
        sendto(sd, "HI", 2, 0, (struct sockaddr *) &server, sizeof(server));
        sleep(5);

    }
    close(sd);

}