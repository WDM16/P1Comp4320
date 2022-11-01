// Will May
#include<iostream>
#include<cstdlib>
#include <random>
using namespace std;
using std::cout;
using std::cin;
using std::endl;


bool gremlin(int randNum) {

    int damagedPacket = randNum;
    return true;
    

}

int generateRandomNum() {
    // Providing a seed value
	srand((unsigned) time(NULL));

	// Get a random number
	int random = 1+ (rand() % 10);

	// Print the random number
	//cout<<random<<endl;
    //According to project description we will pass this number to the gremlin function and can divide by 10
    //as needed if we want to follow the description
    return random;
}

//Jackson - Generate random number between 1 and 10
int main(){
    int a = generateRandomNum();
	cout << gremlin(a);
    
	return 1;
}

