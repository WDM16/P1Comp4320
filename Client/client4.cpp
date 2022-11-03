#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <numeric>
#include <vector>
#include <cstdlib>
#include <netdb.h>
#define PORT 8080
#define TESTFILE "TESTFILE.txt"
using std::cout;
using std::endl;


//Emily Richardson
//Jackson Shaw
//Will May

std::stringstream buffer;
int lprob;
int dprob;
int sockfd;
struct sockaddr_in servaddr;
char buf[512] = {0};


int connect() //Connection to Client 

{
	memset(&servaddr, 0, sizeof(servaddr));


	servaddr.sin_family = AF_INET; 

	servaddr.sin_port = htons(PORT); //Assign the Port Number

	servaddr.sin_addr.s_addr = INADDR_ANY; //Assign IP Address

	return 0;
}

int checkSum(char packet[])
{
	int checksum = 0;
	for (int i = 7; i < 512; i++) //Go through each of the packets in setChecksum
	{
		checksum += packet[i];
	}
	return checksum;
}

//Calculate checksum by adding the bytes of each packet
void setChecksum(char packet[])
{
	int checksum = checkSum(packet);
	char nums[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
	packet[2] = nums[checksum / 10000 % 10];
	packet[3] = nums[checksum / 1000 % 10];
	packet[4] = nums[checksum / 100 % 10];
	packet[5] = nums[checksum / 10 % 10];
	packet[6] = nums[checksum % 10];
	cout << "Checksum: " << std::to_string(checksum) << endl;
}

//User requests for Gremlin
int gremlinProbabilities()
{
	cout << "Enter Packet Damange Probability (0-100): ";

	std::cin >> dprob;

	cout << "Enter Packet Loss Probability (0-100): ";

	std::cin >> lprob;

	cout << "Gremlin probabilities are (" << std::to_string(dprob) << "% damage) and (" << std::to_string(lprob) << "% loss)" << endl;
	
    return 0;
}

//Damage method to damage packets
void damage(char packet[], int value)
{
	for (int i = 0; i < value; i++)
	{
		int randomNum = rand() % 511;
		packet[randomNum] = 'a' + rand() % 26;
	}
	cout << "GREMLIN: Packet damaged " << value << " times" << endl;
}

//Gremlin function
void gremlin(char packet[])
{
	int randomNum = rand() % 100 + 1; //Choose random number from 1-100
	if (randomNum <= dprob) //Checks if the packet got damanged
	    if (randomNum <= dprob)
	    {
		    randomNum = rand() % 10 + 1;
            if (randomNum == 10)
		    {
			    damage(packet, 3);
		    }		
            else if (randomNum >= 8)
		    {
			    damage(packet, 2);
		    }
            else
		    {
			    damage(packet, 1);
		    }
            return;
        }
	randomNum = rand() % 100 + 1;
	if (randomNum <= lprob) { //Packet lost due to Gremlin
		cout << "GREMLIN: Packet lost" << endl;
		packet[1] = 'N';
	}
	else //Packet Successfully Delivered
	{
		cout << "GREMLIN: Packet correctly delivered" << endl;
	}
}

//Method to send the packet
void sendPacket(char packet[])
{
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { //Failed to create socket
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}
	sendto(sockfd, (const char *)packet, 512, 0, (const struct sockaddr *)&servaddr, sizeof(servaddr)); //Sends packet
	cout << "Packet sent" << endl;
	int n;
	socklen_t len;
	n = recvfrom(sockfd, (char *)buf, 512, MSG_WAITALL, (struct sockaddr *)&servaddr, &len);
    buf[n] = '\0';
	printf("Server : %s\n", buf);
	close(sockfd);
}

// create packets
void createPackets()
{
	int characterCount = 0;

	char sequenceNumber = '0';

	int headerSize = 7;

	while (characterCount < buffer.str().length())
	{
		char packet[512];

		int characterCountInBuffer = headerSize; // start after header

		// setup header
		packet[0] = sequenceNumber;

		packet[1] = 'Y'; // Y means ok, N means error

		cout << "writing data to packet #" << sequenceNumber << endl;

		// loop until packet is full or buffer is completely read
		while (characterCount < buffer.str().length() && characterCountInBuffer < 512)
		{
			packet[characterCountInBuffer] = buffer.str()[characterCount];

			characterCountInBuffer++;

			characterCount++;
		}

		// check if packet is full and fill with null
		while (characterCountInBuffer < 512)
		{
			packet[characterCountInBuffer] = '\0';

			characterCountInBuffer++;
		}

		setChecksum(packet);
		gremlin(packet);
		sequenceNumber = (sequenceNumber == '0') ? '1' : '0';

		// if packet not lost
		if (packet[1] == 'Y')
		{
			// show packet info
			std::string packetString = "";

			for (int i = 0; i < 48; i++)
			{
				packetString += packet[i];
			}

			cout << "Packet #" << sequenceNumber << " to be sent: " << packetString << endl;

			sendPacket(packet);
		}
	}

	// create blank ending packet
	cout << "Creating end packet" << endl;

	char endPacket[] = {'\0'};

	sendPacket(endPacket);
}

// read test file into buffer
bool readFile(std::string fileName)
{
	std::fstream file(fileName);

	if (!file.is_open())
	{
		cout << "File is not found" << endl;

		return false;
	}

	buffer << file.rdbuf();

	return true;
}

bool sendRequest()
{
	char request[] = "GET TestFile";

	sendto(sockfd, (const char *)request, strlen(request),
		   0, (const struct sockaddr *)&servaddr,
		   sizeof(servaddr));

	cout << "Sending: " << request << endl;

	cout << endl;

	return true;
}

int main(int argc, char const *argv[])
{
	srand(time(0));

	sockfd = connect();

	if (sockfd != 0)
	{
		return -1;
	}

	if (!readFile(TESTFILE))
	{
		return -1;
	}

	gremlinProbabilities();

	if (!sendRequest())
	{
		return -1;
	}

	createPackets();
	return 0;
}