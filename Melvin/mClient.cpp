/**
 * @file UDPClient.cpp
 * @author Melvin Moreno (mem0282@auburn.edu.com)
 * @brief FTP Client over UDP
 * @version 0.1 COMP4320 Project 1
 * @date 2022-10-19
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
#define TESTFILE "/Users/willmay/Desktop/Introduction To Computer Networks (Comp 4320)/P1Comp4320/Test Files"
using std::cout;
using std::cin;
using std::endl;

std::stringstream buffer;
int damageProbability;
int lossProbability;
int socketFD;
struct sockaddr_in serverAddress;
struct hostent *server;
char packetBuffer[512];


// Connect to the client
int connectToServer() {
    memset(&serverAddress, 0, sizeof(serverAddress));

    // Bind the socket to the port
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    return 0;
}

// Calculate the checksum
int calculateCheckSum(char packet[]) {
    int sum = 0;
    for (int i = 0; i < 512; i++) {
        sum += packet[i];
    }
    return sum;
}

// Calculate the checksum by summing the bytes.
void setCheckSum(char packet[]) {
    int sum = calculateCheckSum(packet);
    
    // Set the checksum
    char digits[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	packet[2] = digits[sum / 10000 % 10];
	packet[3] = digits[sum / 1000 % 10];
	packet[4] = digits[sum / 100 % 10];
	packet[5] = digits[sum / 10 % 10];
	packet[6] = digits[sum % 10];
    cout << "Checksum: " << packet[2] << packet[3] << packet[4] << packet[5] << packet[6] << endl;
}

// Allow the probabilities of damaged and lost packets to be input as arguments when the program is run.
int setProbabilities() {
    cout << "Enter the probability of damaged packets: ";
    cin >> damageProbability;
    cout << "Enter the probability of lost packets: ";
    cin >> lossProbability;
    cout << "Damage Probability: " << damageProbability << endl;
    cout << "Loss Probability: " << lossProbability << endl;
    return 0;
}

// Damage the packet
void damagePacket(char packet[], int amount) {
    int random = rand() % 100;
    if (random < damageProbability) {
        int randomIndex = rand() % 512;
        packet[randomIndex] = 'X' + rand() % 26;
    }
    cout << "GREMLIN: Packet damaged." << amount << " times" << endl;
}

// Implement Gremlin to damage, lose, or successfully send the packet.
void gremlin(char packet[]) {
    int random = rand() % 100;
    // Randomly damage the packet
    if (random < damageProbability) {
        damagePacket(packet, 1);
    }
    // Randomly lose the packet and set to null.
    if (random < lossProbability) {
        cout << "GREMLIN: Packet lost." << endl;
        packet = NULL;
        return;
    }
    // Send the packet
    else {
        cout << "GREMLIN: Packet successfully sent.";
    }
}
// Send the packet
void sendPacket(char packet[]) {
    // Send the packet
    int sent = sendto(socketFD, packet, 512, 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    if (sent < 0) {
        cout << "ERROR: Failed to send packet." << endl;
        exit(1);
    }
    cout << "Packet sent." << endl;
}


// Create the packets
void createPacket(char packet[], int sequenceNumber, int packetSize) {
    int totalCharCount = 0;
	char sequenceNum = '0';
	int headerSize = 7;

	while (totalCharCount < buffer.str().length())
	{
		char packet[512];
		int charCountInBuffer = headerSize; // start after header

		// setup header
		packet[0] = sequenceNum;
		packet[1] = 'A'; // Error protocol: A if OK, B if ERROR

		cout << "writing data to packet #" << sequenceNum << endl;

		// loop until packet is full or buffer is completely read
		while (totalCharCount < buffer.str().length() && charCountInBuffer < 128)
		{
			packet[charCountInBuffer] = buffer.str()[totalCharCount];
			charCountInBuffer++;
			totalCharCount++;
		}

		// check if packet is full and fill with null
		while (charCountInBuffer < 512)
		{
			packet[charCountInBuffer] = '\0';
			charCountInBuffer++;
		}

		setCheckSum(packet);
		gremlin(packet);
		sequenceNum = (sequenceNum == '0') ? '1' : '0';

		// if packet not lost
		if (packet[1] == 'A')
		{
			// show packet info
			std::string packetString = "";
			for (int i = 0; i < 48; i++)
			{
				packetString += packet[i];
			}

			cout << "Packet #" << sequenceNum << " to be sent: " << packetString << endl;
			sendPacket(packet);
		}
	}

	// create blank ending packet
	cout << "Creating end packet" << endl;
	char endPacket[] = {'\0'};
	sendPacket(endPacket);
}

// Read the test file into buffer
bool readFile() {
    std::ifstream file(TESTFILE);
    if (file.is_open()) {
        buffer << file.rdbuf();
        file.close();
        return true;
    }
    else {
        cout << "ERROR: Failed to open file." << endl;
        return false;
    }
}

// Send Request
bool sendRequest() {
    char request[] = "GET TestFile";
    sendto(socketFD, request, 512, 0, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    cout << "Sending request..." << request << endl;
    cout << endl;
    return true;
}

//main function
int main(int argc, char *argv[]) {
    {
	srand(time(0));
	socketFD = connectToServer();
	if (socketFD != 0)
	{
		return -1;
	}
    
    cout << "Successfully connected to server..." << endl;

	setProbabilities();
    cout << "Successfully set Gremlin probabilities...";

	if (!sendRequest())
	{
		return -1;
	}

	createPacket(packetBuffer, 0, 512);

    cout << "Successfully created and sent the packet..." << endl;
}
}