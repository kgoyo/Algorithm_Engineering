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
    if (key < nodeArray[cIndex].value) {
        return -1;
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

    /*test print
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
     */

    return nodeArray;
}

void binarySearchInOrder(int array[], int LENGTH, int numRuns, fstream& file) {

    srand((unsigned)time(NULL)); //init seed
    int randomArray[numRuns];
    for (int i = 0; i<numRuns; i++) {
        randomArray[i] = rand() % array[LENGTH-1];
    }
    int rootIndex;
    Node* nodeArray = constructInOrder(array,LENGTH,rootIndex);

    //start measure stuff here
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

    for (int i = 0; i< numRuns; i++) {
        int index = BSTSearch(rootIndex,nodeArray,randomArray[i]);
        /*cout << "key: " << randomArray[i] << " index: " << index;
        if (index != -1) {
            cout << " best match: " << nodeArray[index].value << endl;
        }*/
    }

    //end measure stuff here
#ifdef TIME
    auto end = Clock::now();
        file << LENGTH << " " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << endl;
#endif
#ifdef LINUX
    /* Stop counters and store results in values */
    retval = PAPI_stop_counters(values,NUM_EVENTS);
	file << LENGTH << " " << values[0] / numRuns << endl;
#endif

    delete nodeArray;
}


int dfsRecurse(int array[], Node nodes[], int &count, int low, int high) {
    int midpoint = low + (high - low) / 2;
    int myIndex = count;
    nodes[myIndex].value = array[midpoint];
    if (low < midpoint) {
        count++;
        nodes[myIndex].left = dfsRecurse(array, nodes, count, low, midpoint-1);
    } else {
        nodes[myIndex].left = -1; //used to indicate null pointer
    }
    if (high > midpoint) {
        count++;
        nodes[myIndex].right = dfsRecurse(array, nodes, count, midpoint+1, high);
    } else {
        nodes[myIndex].right = -1; //used to indicate null pointer
    }
    return myIndex;
}

Node* constructDFS(int array[], int LENGTH, int& rootIndex) {
    int count = 0;
    Node* nodeArray = new Node[LENGTH];
    rootIndex = dfsRecurse(array, nodeArray,count,0,LENGTH-1);
    /*
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
     */

    return nodeArray;
}


void binarySearchDFS(int array[], int LENGTH, int numRuns, fstream& file) {
    srand((unsigned)time(NULL)); //init seed
    int randomArray[numRuns];
    for (int i = 0; i<numRuns; i++) {
        randomArray[i] = rand() % array[LENGTH-1];
    }
    int rootIndex;
    Node* nodeArray = constructDFS(array,LENGTH,rootIndex);

    //start measure stuff here
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

    for (int i = 0; i< numRuns; i++) {
        int index = BSTSearch(rootIndex,nodeArray,randomArray[i]);
        /*cout << "key: " << randomArray[i] << " index: " << index;
        if (index != -1) {
            cout << " best match: " << nodeArray[index].value << endl;
        }*/
    }

    //end measure stuff here
#ifdef TIME
    auto end = Clock::now();
        file << LENGTH << " " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << endl;
#endif
#ifdef LINUX
    /* Stop counters and store results in values */
    retval = PAPI_stop_counters(values,NUM_EVENTS);
	file << LENGTH << " " << values[0] / numRuns << endl;
#endif

    delete nodeArray;
}

/*
int bfsRecurse(int array[], Node nodes[], int bitString, int low, int high, int nullCountArray[], int currentLayer) {
    int midpoint = low + (high - low) / 2;
    int myIndex = bitString-1;
    nodes[myIndex].value = array[midpoint];
    if (low < midpoint) {
        nodes[myIndex].left = bfsRecurse(array, nodes, (bitString << 1) - nullCountArray[currentLayer], low, midpoint-1, nullCountArray, currentLayer+1);
    } else {
        nodes[myIndex].left = -1; //used to indicate null pointer
        nullCountArray[currentLayer]++;
    }
    if (high > midpoint) {
        nodes[myIndex].right = bfsRecurse(array, nodes, ((bitString << 1) + 1)  - nullCountArray[currentLayer], midpoint+1, high, nullCountArray, currentLayer+1);
    } else {
        nodes[myIndex].right = -1; //used to indicate null pointer
        nullCountArray[currentLayer]++;
    }
    return myIndex;
}

Node* constructBFS(int array[], int LENGTH, int& rootIndex) {
    int rootBitString = 1;
    Node* nodeArray = new Node[LENGTH];
    int nullCountArray[LENGTH]; //log(length) should be enough but length is plenty, Gerth style
    for (int i = 0; i<LENGTH;i++) {
        nullCountArray[i] = 0;
    }
    rootIndex = bfsRecurse(array, nodeArray,rootBitString,0,LENGTH-1,nullCountArray,0);
    /*
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
*/

void bfsFirstPass(int layerCountArray[], int currentLayer, int low, int high) {
    int midpoint = low + (high - low) / 2;
    layerCountArray[currentLayer]++;
    if (low < midpoint) {
        bfsFirstPass(layerCountArray, currentLayer+1 , low, midpoint-1);
    }
    if (high > midpoint) {
        bfsFirstPass(layerCountArray, currentLayer+1, midpoint+1, high);
    }
}

int bfsRecurse(int array[], Node nodes[], int layerCountArray[], int offsetInLayer[], int currentLayer, int low, int high) {
    int midpoint = low + (high - low) / 2;

    //calculate index and maintain offsetInLayer array
    int myIndex = 0;
    for (int i = 0; i < currentLayer; i++) {
        myIndex+= layerCountArray[i];
    }
    myIndex+= offsetInLayer[currentLayer];
    offsetInLayer[currentLayer]++;

    //set value
    nodes[myIndex].value = array[midpoint];

    //recurse
    if (low < midpoint) {
        nodes[myIndex].left = bfsRecurse(array, nodes, layerCountArray, offsetInLayer, currentLayer+1, low, midpoint-1);
    } else {
        nodes[myIndex].left = -1; //used to indicate null pointer
    }
    if (high > midpoint) {
        nodes[myIndex].right = bfsRecurse(array, nodes, layerCountArray, offsetInLayer, currentLayer+1, midpoint+1, high);
    } else {
        nodes[myIndex].right = -1; //used to indicate null pointer
    }

    //return index
    cout << "myIndex: " << myIndex << endl;
    return myIndex;
}

Node* constructBFS(int array[], int LENGTH, int& rootIndex) {
    int layerCountArray[LENGTH]; //actuallength leeded is size of tree which is <LENGTH
    for (int i =0; i<LENGTH;i++) {
        layerCountArray[i] = 0;
    }
    bfsFirstPass(layerCountArray,0,0,LENGTH-1);
    int offsetInLayer[LENGTH]; //actuallength leeded is size of tree which is <LENGTH
    for (int i =0; i<LENGTH;i++) {
        offsetInLayer[i] = 0;
    }
    Node* nodeArray = new Node[LENGTH];
    rootIndex = bfsRecurse(array,nodeArray,layerCountArray,offsetInLayer,0,0,LENGTH-1);

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

typedef struct NodeTemp {
    int value;
    int left;
    int right;
    int outputIndex;
}NodeTemp;

//copypasted inorder with nodetemp
int inOrderRecurseVEB(int array[], NodeTemp nodes[], int low, int high, int currentLayer, int& height) {

    //VEB specific stuff
    if (currentLayer > height) {
        height = currentLayer;
    }

    int midpoint = low + (high - low) / 2;
    nodes[midpoint].value = array[midpoint];
    if (low < midpoint) {
        nodes[midpoint].left = inOrderRecurseVEB(array, nodes, low, midpoint-1, currentLayer+1, height);
    } else {
        nodes[midpoint].left = -1; //used to indicate null pointer
    }
    if (high > midpoint) {
        nodes[midpoint].right = inOrderRecurseVEB(array, nodes, midpoint+1, high, currentLayer+1, height);
    } else {
        nodes[midpoint].right = -1; //used to indicate null pointer
    }
    return midpoint;
}

//signature so that we can do the mutually recursiveness
void assignIndexVEB(NodeTemp nodes[], int currentHeight, int& count, int subtreeRootIndex);


void traverseVEB(NodeTemp nodes[], int currentNodeIndex, int height, int goalDepth, int currentDepth, int& count) {
    if (currentDepth == goalDepth) {
        assignIndexVEB(nodes, height - (height/2), count, currentNodeIndex);
    } else {
        if (nodes[currentNodeIndex].left != -1) {
            traverseVEB(nodes, nodes[currentNodeIndex].left, height, goalDepth, currentDepth+1, count);
        }
        if (nodes[currentNodeIndex].right != -1) {
            traverseVEB(nodes, nodes[currentNodeIndex].right, height, goalDepth, currentDepth+1, count);
        }
    }
}

void assignIndexVEB(NodeTemp nodes[], int currentHeight, int& count, int subtreeRootIndex) {
    if (currentHeight == 1) {
        //base case
        nodes[subtreeRootIndex].outputIndex = count;
        count++;
    } else {
        //recursion case

        //top tree
        assignIndexVEB(nodes, currentHeight/2, count, subtreeRootIndex);

        //go down tree h/2 steps and call assignIndexVEB on those
        traverseVEB(nodes, subtreeRootIndex, currentHeight, currentHeight/2 + 1, 1, count);
    }
}

int mapToNodeArrayVEB(NodeTemp src[], Node dst[], int currentIndexInSrc) {
    int dstIndex = src[currentIndexInSrc].outputIndex;
    dst[dstIndex].value = src[currentIndexInSrc].value;

    //recurse left
    if (src[currentIndexInSrc].left != -1) {
        dst[dstIndex].left = mapToNodeArrayVEB(src, dst, src[currentIndexInSrc].left);
    } else {
        dst[dstIndex].left = -1;
    }

    //recurse right
    if (src[currentIndexInSrc].right != -1) {
        dst[dstIndex].right = mapToNodeArrayVEB(src, dst, src[currentIndexInSrc].right);
    } else {
        dst[dstIndex].right = -1;
    }

    return dstIndex;
}

Node* constructVEB(int array[], int LENGTH, int& rootIndex) {
    NodeTemp* tempNodeArray = new NodeTemp[LENGTH];
    for (int i=0; i<LENGTH;i++) {
        tempNodeArray[i].outputIndex = -1;
    }
    int height = 0;
    int tempRoot = inOrderRecurseVEB(array, tempNodeArray,0, LENGTH-1,1,height);
    int count = 0;
    assignIndexVEB(tempNodeArray, height, count, tempRoot);
//
//    cout << "output:";
//    for (int i=0; i<LENGTH; i++) {
//        cout << " [\"" << i << "\"," << tempNodeArray[i].outputIndex << "," << tempNodeArray[i].left << "," << tempNodeArray[i].right << "]" ;
//    }
//    cout << endl;
    Node* nodeArray = new Node[LENGTH];
    rootIndex = mapToNodeArrayVEB(tempNodeArray, nodeArray,tempRoot);
    delete tempNodeArray;


//    cout << "input:";
//    for (int i=0; i<LENGTH; i++) {
//        cout << " " << array[i];
//    }
//    cout << endl;
//
//    cout << "output:";
//    for (int i=0; i<LENGTH; i++) {
//        cout << " [\"" << i << "\"," << nodeArray[i].value << "," << nodeArray[i].left << "," << nodeArray[i].right << "]" ;
//    }
//    cout << endl;


    return nodeArray;
}

void nodeArrayToDot(Node nodes[], int LENGTH) {
    fstream dotFile;
    dotFile.open("nodes.dot", ios::out);
    dotFile << "graph {" << endl;
    for (int i=0; i<LENGTH; i++) {
        cout << i << endl;
        if (nodes[i].left != -1) {
            dotFile << i << " -- " << nodes[i].left << ";" << endl;
        }
        if (nodes[i].right != -1) {
            dotFile << i << " -- " << nodes[i].right << ";" << endl;
        }
    }
    dotFile << "}" << endl;
    dotFile.close();
}

void binarySearchBFS(int array[], int LENGTH, int numRuns, fstream& file) {
    srand((unsigned)time(NULL)); //init seed
    int randomArray[numRuns];
    for (int i = 0; i<numRuns; i++) {
        randomArray[i] = rand() % array[LENGTH-1];
    }
    int rootIndex;
    Node* nodeArray = constructBFS(array,LENGTH,rootIndex);

    //start measure stuff here
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

    for (int i = 0; i< numRuns; i++) {
        int index = BSTSearch(rootIndex,nodeArray,randomArray[i]);
        /*cout << "key: " << randomArray[i] << " index: " << index;
        if (index != -1) {
            cout << " best match: " << nodeArray[index].value << endl;
        }*/
    }

    //end measure stuff here
#ifdef TIME
    auto end = Clock::now();
        file << LENGTH << " " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << endl;
#endif
#ifdef LINUX
    /* Stop counters and store results in values */
    retval = PAPI_stop_counters(values,NUM_EVENTS);
	file << LENGTH << " " << values[0] / numRuns << endl;
#endif

    delete nodeArray;
}

void binarySearchVEB(int array[], int LENGTH, int numRuns, fstream& file) {
    srand((unsigned)time(NULL)); //init seed
    int randomArray[numRuns];
    for (int i = 0; i<numRuns; i++) {
        randomArray[i] = rand() % array[LENGTH-1];
    }
    int rootIndex;
    Node* nodeArray = constructVEB(array,LENGTH,rootIndex);

    //start measure stuff here
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

    for (int i = 0; i< numRuns; i++) {
        int index = BSTSearch(rootIndex,nodeArray,randomArray[i]);
        /*cout << "key: " << randomArray[i] << " index: " << index;
        if (index != -1) {
            cout << " best match: " << nodeArray[index].value << endl;
        }*/
    }

    //end measure stuff here
#ifdef TIME
    auto end = Clock::now();
        file << LENGTH << " " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << endl;
#endif
#ifdef LINUX
    /* Stop counters and store results in values */
    retval = PAPI_stop_counters(values,NUM_EVENTS);
	file << LENGTH << " " << values[0] / numRuns << endl;
#endif

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
	for (int i = 0; i <= 3;i++) {
        cout << "n=" << pow(10,i) << endl;
		//binarySearchSorted(array, pow(10, i), 500, outputFile);
        //binarySearchInOrder(array, pow(10,i), 100, outputFile);
        //binarySearchBFS(array, pow(10,i), 100, outputFile);
        //binarySearchDFS(array, pow(10,i), 100, outputFile);
        //binarySearchVEB(array, pow(10,i), 100, outputFile);
	}
    int rootIndex;
    int L = 16;
    Node* nodeArray = constructVEB(array,L,rootIndex);
    nodeArrayToDot(nodeArray,L);
	outputFile.close();
	//delete array
	delete[] array;
}
