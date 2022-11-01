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

using std::cout;
using std::cin;
using std::endl;

char packetBuffer[512];

ifstream openFile(char* fileName) {
    ifstream infile;
    infile.open (fileName, ios::in);

    if (!infile.is_open()) {
        cout << "Cannot Open File";
        exit(1);
    }
    else {
        cout << "File Succsessfully Opened\n";
    }
    return infile;
}

int main(int argc, char **argv) {
    int sd;
    struct sockaddr_in server;
    struct hostent *hp;

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&server, sizeof(server)); //Covered in lecture, said we need this

    server.sin_family = AF_INET;
    server.sin_port = htons(12345); //Port number
    hp = gethostbyname("10.0.0.202"); //Server IP address
    bcopy(hp->h_addr, &(server.sin_addr), hp->h_length); //Binds IP address and Port number? It copys from location one to location two

    for(;;) {
        //Sends messgae to server
        sendto(sd, "HIS", 2, 0, (struct sockaddr *) &server, sizeof(server));
        sleep(5);

    }
    close(sd);

}
//put gremlin in client
int setProbabilities() {
    cout << "Enter the probability of damaged packets (0-100): ";
    cin >> damageProb;
    cout << "Enter the probability of lost packets (0-100): ";
    cin >> lossProb;
    return 0;
}