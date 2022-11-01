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
int main() {
    int n, sd;
    struct sockaddr_in server;
    char buf[512];

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(12345); //Server port number

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    bind(sd, (struct sockaddr *)&server, sizeof(server));

    for(;;) {
        //Recieves message from clients 
        n = recv (sd, buf, sizeof(buf), 0);
        buf[n] = '\0';
        printf("Received: %s\n", buf);

    }
    close(sd);
    return 0;
}