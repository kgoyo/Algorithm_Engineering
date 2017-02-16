#include <cstdlib>			//needed for random numbers
#include <iostream>
#include <ctime>
#include <cmath>			//library for math stuff
#include <fstream>
#include <chrono>           //Quality clock
#ifdef TIME
    typedef std::chrono::high_resolution_clock Clock;
#endif

//#ifdef LINUX
#include <papi.h> 			//PAPI performance measurement
#define NUM_EVENTS 1
//#endif

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
	
	//#ifdef LINUX
        long long values[NUM_EVENTS];
        unsigned int Events[NUM_EVENTS]={PAPI_BR_MSP};
        /* Initialize the Library */
        int retval = PAPI_library_init(PAPI_VER_CURRENT);
        /* Start the counters */
        PAPI_start_counters((int*)Events,NUM_EVENTS);
        PAPI_read_counters(values,NUM_EVENTS);
	//#endif

    #ifdef TIME
        auto start = Clock::now();
    #endif

	/* What we are monitoring... */
	for (int i = 0; i < numRuns; i++) {

		int key = rand() % array[LENGTH - 1];
		//cout << "searching with key: " << key << endl;

		int low = 0, high = LENGTH - 1, midpoint;

		while (low <= high) {
			midpoint = low + (high - low) / 2;
			if (key == array[midpoint]) {
				//if found
				//cout << "found " << array[midpoint] << endl;
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

		//cout << "found approx " << last << endl;

	}

    #ifdef TIME
        auto end = Clock::now();
        file << LENGTH << " " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / numRuns << endl;
    #endif
	//First line is double for some reason ask Gert!
	//#ifdef LINUX
	/* Stop counters and store results in values */
    retval = PAPI_stop_counters(values,NUM_EVENTS);
	file << LENGTH << " " << values[0] / numRuns << endl;
	//#endif

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
	for (int i = 0; i <= 7;i++) {
        cout << "n=" << pow(10,i) << endl;
		binarySearchSorted(array, pow(10, i), 500, outputFile);
	}
	outputFile.close();

	//delete array
	delete[] array;
}
