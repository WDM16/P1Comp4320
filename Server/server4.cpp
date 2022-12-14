//Emily Richardson
//Jackson Shaw
//Will May

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdexcept>
#define PORT 12345
#define PACKET_SIZE 512
using namespace std;

int sockfd;
struct sockaddr_in cliaddr, servaddr;
int windowSize = 32; //defined in Project2 description
int firstSeq = 0; //seq number of first frame in window
int nSeQ = 0; //sequence number of nth frame in window

//Method to initialize socket, port, IP, etc.
int init() { 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { //Test case
		perror("Failure to create socket.");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY; //Assign IP Address
	servaddr.sin_port = htons(PORT); //Assign Port Number
	if (::bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) { //If can't bind 
		perror("Failure to bind socket.");
		exit(EXIT_FAILURE);
	}
	return 0;
}
 
//Method to calculate the checksum by adding the bytes of the packet body
int checkSum(char buffer[]) {
	int checksum = 0;
	for (int i = 7; i < PACKET_SIZE; i++) {
		checksum += buffer[i];
	}
	return checksum;
}

//Method that compares checksum calculated to the checksum in the header
bool validateChecksum(char buffer[]) {
	try {
		int calculateChecksum = checkSum(buffer);
		string checkSumString;
		for (int i = 2; i < 7; i++) {
			checkSumString += buffer[i];
		}
		int passedChecksum = stoi(checkSumString);
		return calculateChecksum == passedChecksum;
	}
	catch (std::invalid_argument) {
		return false;
	}
}

//Method that writes the buffer body to filestream
int writeFile(ofstream &file, char buffer[]) {
	for (int i = 7; i < PACKET_SIZE; i++) {
		if (buffer[i] != '\0') 
			file << buffer[i];
	}
	return 0;
}

//Method that sends response to client
int sendResponse(char resp[], socklen_t socketLength) {
	sendto(sockfd, (const char *)resp, strlen(resp), 0, (const struct sockaddr *)&cliaddr, socketLength);
	cout << "Sending response: " << resp << endl;
	cout << endl;
	return 0;
}


//Method that listens for message from client and writes the output
int receiveMessage() {
	/*
	Experimental GBN Protocol Implementation Pseudocode
	while(true) {
		seqnum = 0
		windowmax = 31
		send 0-31
		when we recieve an ack the window increases and we send again
		if we timeout or nak then we resend from that number
	}
	*/
	char buffer[PACKET_SIZE] = {0};
	ofstream file;
	socklen_t socketLength;
	int msgLength;
	socketLength = sizeof(cliaddr);
	file.open("OutputFile.txt");
	if (!file.is_open()) { //Checks if the file is not open
		cout << "Failure to open output file." << endl;
		exit(EXIT_FAILURE);
	}
	char previousSequenceNumber;
	while (true) {
		msgLength = recvfrom(sockfd, (char *)buffer, PACKET_SIZE, MSG_WAITALL, (struct sockaddr *)&cliaddr, &socketLength);
		buffer[msgLength] = '\0';
		char sequenceNumber = buffer[0];
		if (sequenceNumber == '\0') {
			break;
		}
		else if (sequenceNumber == previousSequenceNumber) {
			cout << "Packet " << sequenceNumber << " has a duplicate sequence number. A packet was lost." << endl;
		}
		else {
			cout << "Packet No. " << sequenceNumber << " received. Checking for errors..." << endl;

			if (!validateChecksum(buffer) || (sequenceNumber != '0' && sequenceNumber != '1')) {
				cout << "Packet No. " << sequenceNumber << " is damaged." << endl;
			}
			else {
				cout << "Packet No. " << sequenceNumber << " is ok." << endl;
			}
		}
		previousSequenceNumber = sequenceNumber;
		cout << "Printing the first 100 bytes of packet " << sequenceNumber << ":" << endl;
		for (int i = 0; i < 100; i++) {
			cout << buffer[i];
		}
		cout << endl;
		writeFile(file, buffer);
		char resp[18];
		sprintf(resp, "Packet %c received.", sequenceNumber);
		sendResponse(resp, socketLength);
	}
	cout << "Final packet received. Writing file." << endl;
	file.close();
	char successMsg[] = "GET successfully completed";
	sendResponse(successMsg, socketLength);
	return 0;
}

//Main Method of program
int main(int argc, char const *argv[])
{
	cout << "Starting server" << endl;
	init();
	receiveMessage();
	return 0;
}