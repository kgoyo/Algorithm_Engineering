#include <iostream>
#include <cstdlib>			//needed for random numbers
#include <ctime>
#include <cmath>			//library for math stuff
#include <fstream>

using namespace std;

int * loadArrayfromFile(const char filePath[], int& LENGTH) {
	fstream file;
	file.open(filePath, ios::in);
	int a;
	file >> a;
	LENGTH = a;
	cout << "loaded file with " << LENGTH << " values" << endl;
	
	int *array = new int[LENGTH];

	for (int i = 0; i < LENGTH; i++) {
		file >> a;
		array[i] = a;
	}

	return array;
}

int main(int argc, const char* argv[]) {
	int *array;
	int LENGTH;
	array = loadArrayfromFile("randomSortedArray.txt", LENGTH);
	for (int i = 0; i < LENGTH; i++) {
		cout << array[i] << endl;
	}

	//delete array
	delete[] array;
}

