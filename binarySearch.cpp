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


void binarySearchSorted(int array[], int LENGTH, int numRuns) {
	srand((unsigned)time(NULL)); //init seed

	for (int i = 0; i < numRuns; i++) {
		int key = rand() % array[LENGTH - 1];
		cout << "searching with key: " << key << endl;

		int low = 0, high = LENGTH - 1, midpoint;

		while (low <= high) {
			midpoint = low + (high - low) / 2;
			if (key == array[midpoint]) {
				//if found
				cout << "found " << array[midpoint] << endl;
				break;
			}
			else if (key < array[midpoint]) {
				high = midpoint - 1;
			}
			else {
				low = midpoint + 1;
			}
		}
		int last = array[midpoint];
		if (last > key) {
			last = array[midpoint - 1];
		}

		cout << "found approx " << last << endl;
	}
}

int main(int argc, const char* argv[]) {
	int *array;
	int LENGTH;
	array = loadArrayfromFile("randomSortedArray.txt", LENGTH);
	/*
	for (int i = 0; i < LENGTH; i++) {
		cout << array[i] << endl;
	}
	*/

	binarySearchSorted(array, LENGTH, 50);

	//delete array
	delete[] array;
}
