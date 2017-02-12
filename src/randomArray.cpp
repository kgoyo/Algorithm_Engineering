#include <iostream>
#include <cstdlib>			//needed for random numbers
#include <ctime>
#include <cmath>			//library for math stuff
#include <fstream>

using namespace std;

int main(int argc, const char* argv[]) {
	int LENGTH = atoi(argv[1]);
	int lastRand = 0;
	fstream file;
	file.open("randomSortedArray.txt", ios::out);

	file << LENGTH << endl; //first value in the file is the number of values in the array

	srand((unsigned)time(NULL)); //init seed
	for (int i = 0; i < LENGTH; i++) {
		lastRand += rand() % 50 +1;
		file << lastRand << endl;
	}
	file.close();
	cout << "file written"<< endl;
}