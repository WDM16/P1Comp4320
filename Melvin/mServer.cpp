
/**
 * @file UDPServer.cpp
 * @author Melvin Moreno (mem0282@auburn.edu.com)
 * @brief FTP Server over UDP
 * @version 0.1 COMP4320 Project 1
 * @date 2022-10-21
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <numeric>
#include <vector>
#include <cstdlib>
#define PORT 10004
#define PACKET_SIZE 512
using namespace std;

int socketFD;
struct sockaddr_in clientAddress, serverAddress;

// Bind the server to localhost and the port number defined.
int bindServer() {
    if ((socketFD = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddress, 0, sizeof(serverAddress));
    memset(&clientAddress, 0, sizeof(clientAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(socketFD, (const struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}

// Calculate the checksum
int calculateCheckSum(char buffer[]) {
    int sum = 0;
    for (int i = 0; i < PACKET_SIZE; i++) {
        sum += buffer[i];
    }
    return sum;
}

// Compare the checksum to the checksum in the message header
bool compareCheckSum(char buffer[]) {
    int sum = 0;
    for (int i = 0; i < PACKET_SIZE; i++) {
        sum += buffer[i];
    }
    return sum == 0;
}

// Write the contents of the buffer's body to the file
int writeToFile(char buffer[], ofstream &file) {
    for (int i = 0; i < PACKET_SIZE; i++) {
        file << buffer[i];
    }
    return 0;
}

// Send a reponse to the client
int sendReponse(char response[], socklen_t socketLength) {
    if (sendto(socketFD, response, PACKET_SIZE, 0, (struct sockaddr *) &clientAddress, socketLength) < 0) {
        perror("ERROR writing to socket");
        exit(1);
    }
    sendto(socketFD, response, PACKET_SIZE, 0, (struct sockaddr *) &clientAddress, socketLength);
    cout << "sending : " << response << endl;
    cout << endl;
    return 0;
}

// Receive a message from the client and write it to the output file.
int receiveMessage(ofstream &file) {
    socklen_t socketLength = sizeof(clientAddress);
    char buffer[PACKET_SIZE];
    char response[PACKET_SIZE];
    int n;

    // Receive the message
    if ((n = recvfrom(socketFD, buffer, PACKET_SIZE, 0, (struct sockaddr *) &clientAddress, &socketLength)) < 0) {
        perror("ERROR reading from socket");
        exit(1);
    }

    // Write the message to the file
    writeToFile(buffer, file);

    // Send a response to the client
    sendReponse(response, socketLength);

    return 0;
}

// Main function
int main(int argc, char *argv[]) {

    cout << "Starting server..." << endl;

    // Create the socket
    socketFD = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFD < 0) {
        perror("ERROR opening socket");
        exit(1);
    }
    
    cout << "Successfully created the socket..." << endl;

    // Bind the server to the port
    bindServer();

    cout << "Succesfully binded the server..." << endl;

    // Open the output file
    ofstream file;
    file.open("output.txt");

    // Receive the message
    receiveMessage(file);

    cout << "Sucessfully receive the message..." << endl;

    // Close the file
    file.close();

    cout << "Sucessfully closed the file..." << endl;

    // Close the socket
    close(socketFD);

    cout << "Successfully closed the socket..." << endl;
    return 0;
}
Footer
© 2022 GitHub, Inc.
Footer navigation
Terms
Privacy
Security
Status
Docs
Contact GitHub
Pricing
API
Training
Blog
About
COMP4320/UDPServer.cpp at main · MelvinM8/COMP4320