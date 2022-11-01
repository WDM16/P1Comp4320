#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <string>
#include <istream>
#include <sstream>
#include <pthread.h>

double damageProb = 0;
double lostProb = 0;

using std::cout; using std::endl;
using namespace std;

int main() {
    int n, sd;
    struct sockaddr_in server;
    char buf[512];

    ofstream outfile;
    outfile.open("output.txt"); //Creates or Opens outputfile
    if (outfile.is_open() and outfile.good()) { //Checks to see if output file is opened. 
    	cout << "Correctly Opened File" << endl;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(12345); //Server port number

    sd = socket(AF_INET, SOCK_DGRAM, 0);

    if (bind(sd, (struct sockaddr *)&server, sizeof(server)) < 0) { //Checks to see if socket is binded
        cerr << "Error Binding Socket" << endl;
    }

    
    
    for(;;) {
        //Recieves message from client 
        n = recv (sd, buf, sizeof(buf), sizeof(server));
        buf[n] = '\0';
        printf("Received: %s\n", buf);

    }

    close(sd);

    return 0;
}

