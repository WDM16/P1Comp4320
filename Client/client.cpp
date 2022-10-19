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

    bzero(&server, sizeof(server)); //Covered in lecture, said we need this

    server.sin_family = AF_INET;
    server.sin_port = htons(12345); //Port number
    hp = gethostbyname("10.0.0.202"); //Server IP address
    bcopy(hp->h_addr, &(server.sin_addr), hp->h_length); //Binds IP address and Port number?

    for(;;) {
        //Sends messgae to server
        sendto(sd, "HI", 2, 0, (struct sockaddr *) &server, sizeof(server));
        sleep(5);

    }
    close(sd);

}