#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdint>
#include <unistd.h>
#include "../unp.h"
#include <fstream>

double damageProb = 0;
double lostProb = 0;

using std::cout; using std::endl;
using namespace std;

bool isThereError(uint16_t expectedHash, uint8_t *data);

bool isPossibleError(uint16_t expectedHash, uint8_t *data) {
    uint16_t hash = checksum(data, 124);
    if (hash == expectedHash) {
        return false;
    }
    return true;
}

int main() {
    int n, sd;
    struct sockaddr_in server, client;
    int slen = sizeof(client);
    char buf[512];
    uint8_t packet[128];
    uint16_t expectedPacketNumber = 0;

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

    listen(sd, 4);
    
    for(;;) {
        //Recieves message from clients 
        n = recv (sd, buf, sizeof(buf), sizeof(server));
        buf[n] = '\0';
        if (buf[0] != '\0') {
        	cout << "received PUT" << endl;
            break;
        }

    }

    for(;;) {
        n = recvfrom(sd, packet, sizeof(packet), 0, (struct sockaddr*) &client, (socklen_t*)&slen);

        uint8_t firstByte = packet[0];
        uint8_t secondByte = packet[1];
        uint8_t thirdByte = packet[2];
        uint8_t fourthByte = packet[3];

        uint16_t packetNumber = (uint16_t)(secondByte << (uint8_t) 8) | firstByte;
        uint16_t hash = (uint16_t)(fourthByte << (uint8_t) 8) | thirdByte;
        uint8_t *data = &packet[4];

        if (n == 1) {
            break;
        }

        if (packetNumber != expectedPacketNumber) {
            cout << "There is a packet missing: " << expectedPacketNumber << endl;
            expectedPacketNumber = packetNumber;
        }

        if (!(isPossibleError(hash, data))) {
        	cout << "Packet good: " << packetNumber << endl;
			for (int i = 4; i < 52; i++) {
				cout << (char) packet[i];
			}
        }
        else {
            cout << "This packet is corrupted: " << packetNumber << endl;
        }

        for (int i = 4; i < n; i++) {
        	//cout << (char) packet[i];
			if (packet[i] != 0)
        		outfile << (char) packet[i];
		}
        //outfile.write((char*)data, sizeof(data));

        expectedPacketNumber++;
    }

    // sending final PUT message and closing the socket and file
    cout << "File transfer complete" << endl;

    sendto(sd, "PUT successfully completed", 26, 0, (struct sockaddr *) &client, sizeof(client));
    close(sd);
    outfile.close();
    return 0;
}

