#include <cstdlib>			//needed for random numbers
#include <iostream>
#include <ctime>
#include <cmath>			//library for math stuff
#include <fstream>
#include <cstring>

#ifdef TIME
    #include <chrono>           //Quality clock
    typedef std::chrono::high_resolution_clock Clock;
#endif

#ifdef LINUX
#include <papi.h> 			//PAPI performance measurement
#define NUM_EVENTS 1
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
    int randomArray[numRuns];
    for(int i=0 ; i < numRuns; i++){
        randomArray[i] = rand() % array[LENGTH - 1];
    }
	
	#ifdef LINUX
        long long values[NUM_EVENTS];
        #ifdef BRANCHMSP
            unsigned int Events[NUM_EVENTS]={PAPI_BR_MSP};
        #endif
        #ifdef BRANCHCOUNT
            unsigned int Events[NUM_EVENTS]={PAPI_BR_CN};
        #endif
        #ifdef L1
            unsigned int Events[NUM_EVENTS]={PAPI_L1_TCM};
        #endif
        #ifdef L2
            unsigned int Events[NUM_EVENTS]={PAPI_L2_TCM};
        #endif
        #ifdef L3
            unsigned int Events[NUM_EVENTS]={PAPI_L3_TCM};
        #endif
        #ifdef INS
            unsigned int Events[NUM_EVENTS]={PAPI_TOT_INS};
        #endif
        /* Initialize the Library */
        int retval = PAPI_library_init(PAPI_VER_CURRENT);
        /* Start the counters */
        PAPI_start_counters((int*)Events,NUM_EVENTS);
        PAPI_read_counters(values,NUM_EVENTS);
	#endif

    #ifdef TIME
        auto start = Clock::now();
    #endif

	/* What we are monitoring... */
	for (int i = 0; i < numRuns; i++) {
        int key = randomArray[i];
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
	#ifdef LINUX
	/* Stop counters and store results in values */
    retval = PAPI_stop_counters(values,NUM_EVENTS);
	file << LENGTH << " " << values[0] / numRuns << endl;
    #endif

}

typedef struct Node {
    int value;
    int left;
    int right;
}Node;

int BSTSearch(int rootIndex, Node nodeArray[], int key) {
    if (key < nodeArray[0].value){
        return -1;
    }
    int cIndex = rootIndex;
    int lIndex = cIndex;
    while(nodeArray[cIndex].value != key) {
        if (key < nodeArray[cIndex].value) {
            //recurse left
            if (nodeArray[cIndex].left != -1) {
                lIndex = cIndex;
                cIndex = nodeArray[cIndex].left;
            } else {
                //no child
                break;
            }
        } else {
            //recurse right
            if (nodeArray[cIndex].right != -1) {
                lIndex =cIndex;
                cIndex = nodeArray[cIndex].right;
            } else {
                //no child
                break;
            }
        }
    }

    //check if we went too far
    if (nodeArray[lIndex].value < nodeArray[cIndex].value) {
        cIndex = lIndex;
    }
    return cIndex;
}

int inOrderRecurse(int array[], Node nodes[], int low, int high) {
    int midpoint = low + (high - low) / 2;
    nodes[midpoint].value = array[midpoint];
    if (low < midpoint) {
        nodes[midpoint].left = inOrderRecurse(array, nodes, low, midpoint-1);
    } else {
        nodes[midpoint].left = -1; //used to indicate null pointer
    }
    if (high > midpoint) {
        nodes[midpoint].right = inOrderRecurse(array, nodes, midpoint+1, high);
    } else {
        nodes[midpoint].right = -1; //used to indicate null pointer
    }
    return midpoint;
}

Node* constructInOrder(int array[], int LENGTH, int& rootIndex) {
    Node* nodeArray = new Node[LENGTH];
    rootIndex = inOrderRecurse(array, nodeArray,0, LENGTH-1);

    //test print
    cout << "input:";
    for (int i=0; i<LENGTH; i++) {
        cout << " " << array[i];
    }
    cout << endl;

    cout << "output:";
    for (int i=0; i<LENGTH; i++) {
        cout << " [\"" << i << "\"," << nodeArray[i].value << "," << nodeArray[i].left << "," << nodeArray[i].right << "]" ;
    }
    cout << endl;

    return nodeArray;
}

void binarySearchInOrder(int array[], int LENGTH, int numRuns, fstream& fil) {

    srand((unsigned)time(NULL)); //init seed
    int randomArray[numRuns];
    for (int i = 0; i<numRuns; i++) {
        randomArray[i] = rand() % array[LENGTH-1];
    }
    int rootIndex;
    Node* nodeArray = constructInOrder(array,LENGTH,rootIndex);

    //start measure stuff here

    for (int i = 0; i< numRuns; i++) {
        int index = BSTSearch(rootIndex,nodeArray,randomArray[i]);
        cout << "key: " << randomArray[i] << " index: " << index;
        if (index != -1) {
            cout << " best match: " << nodeArray[index].value << endl;
        }
    }

    //end measure stuff here

    delete nodeArray;
}


int main(int argc, const char* argv[]) {
	int *array;
	int LENGTH;
    string filename;
    #ifdef LINUX
        filename="papi";
        #ifdef BRANCHMSP
            filename="papi_branchMSP";
        #endif
        #ifdef BRANCHCOUNT
            filename="papi_branchCN";
        #endif
        #ifdef L1
            filename="papi_L1";
        #endif
        #ifdef L2
            filename="papi_L2";
        #endif
        #ifdef L3
            filename="papi_L3";
        #endif
        #ifdef INS
            filename="papi_totalINS";
        #endif
    #endif
    #ifdef TIME
        filename="time";
    #endif
    string tmp ="./out/data/"+filename+".txt";
    char tab2[1024];
    strncpy(tab2, tmp.c_str(), sizeof(tab2));
    tab2[sizeof(tab2) - 1] = 0;
	array = loadArrayfromFile("./out/randomSortedArray.txt", LENGTH);
	/*
	for (int i = 0; i < LENGTH; i++) {
		cout << array[i] << endl;
	}
	*/

	fstream outputFile;
	outputFile.open(tab2, ios::out);
	for (int i = 0; i <= 7;i++) {
        cout << "n=" << pow(10,i) << endl;
		//binarySearchSorted(array, pow(10, i), 500, outputFile);
	}
    binarySearchInOrder(array, 10, 1, outputFile);
	outputFile.close();
	//delete array
	delete[] array;
}
