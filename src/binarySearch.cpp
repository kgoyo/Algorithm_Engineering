#include <iostream>
#include <cstdlib>			//needed for random numbers
#include <ctime>
#include <cmath>			//library for math stuff
#include <fstream>

#ifdef LINUX
#include <papi.h> 			//PAPI performance measurement
#define NUM_EVENTS 3
#endif

using namespace std;

int * loadArrayfromFile(const char filePath[], int& LENGTH) {
	fstream file;
	file.open(filePath, ios::in);
	int a;
	file >> a;
	LENGTH = a;
	//cout << "loaded file with " << LENGTH << " values" << endl;
	
	int *array = new int[LENGTH];

	for (int i = 0; i < LENGTH; i++) {
		file >> a;
		array[i] = a;
	}

	return array;
}


void binarySearchSorted(int array[], int LENGTH, int numRuns, fstream& file) {
	srand((unsigned)time(NULL)); //init seed
	
	#ifdef LINUX
	long long values[numRuns][NUM_EVENTS];
	unsigned int Events[NUM_EVENTS]={PAPI_TOT_INS,PAPI_TOT_CYC,PAPI_BR_MSP};
	/* Initialize the Library */
	int retval = PAPI_library_init(PAPI_VER_CURRENT);
	#endif

	/* What we are monitoring... */
	for (int i = 0; i < numRuns; i++) {
		
		#ifdef LINUX
		/* Start the counters */
		PAPI_start_counters((int*)Events,NUM_EVENTS);
		#endif
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

		#ifdef LINUX
		/* Stop counters and store results in values */
		retval = PAPI_stop_counters(values[i],NUM_EVENTS);
		#endif
	}
	
	//First line is double for some reason ask Gert!
	#ifdef LINUX
	int sum = 0;
	for(int i = 0; i < numRuns; i++){
		for(int k=0; k < NUM_EVENTS; k++){
			cout << values[i][k] << endl;
			
		}
		sum += values[i][2];
		cout << endl;
	}
	file << LENGTH << " " << sum / numRuns << endl;
	#endif

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

	fstream outputFile;
	outputFile.open("data.txt", ios::out);
	for (int i = 0; i <= 6;i++) {
		binarySearchSorted(array, pow(10,i), 50, outputFile);
	}
	outputFile.close();

	//delete array
	delete[] array;
}
