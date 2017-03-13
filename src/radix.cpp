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

const unsigned char INTSIZE = 32;
unsigned int arrayLength;

unsigned int getKey(unsigned int value, unsigned int offset, unsigned int mask) {
    return (value >> offset) & mask;
}

void countingSort(unsigned int array[], unsigned int out[], unsigned int mask, unsigned int offset) {
    unsigned int c[mask + 1];
    for (int i = 0; i <= mask; i++) {
        c[i]=0;
    }

    for (int i = 0; i < arrayLength; i++) {
        unsigned int value = getKey(array[i],offset,mask);
        c[value]++;
    }

    unsigned int total = 0;
    for (int i = 0; i <= mask; i++) {
        unsigned int tmp = c[i];
        c[i] = total;
        total += tmp;
    }

    for (int i = 0; i < arrayLength; i++) {
        unsigned int key = getKey(array[i],offset,mask);
        out[c[key]] = array[i];
        c[key]++;
    }
}

void radixSort(unsigned int in[], unsigned int out[], unsigned int maskSize) {
    unsigned int mask = (1 << maskSize) -1; //2^maskSize - 1
    for (int i=0; i<=INTSIZE-maskSize; i+=maskSize) {
        countingSort(in, out, mask,i);
        unsigned int* tmp = &(*in);
        in = &(*out);
        out = &(*tmp);
    }
    unsigned int* tmp = &(*in);
    in = &(*out);
    out = &(*tmp);
}

void testRadix(int LENGTH, unsigned int maskSize, fstream& file) {
    //init
    arrayLength = LENGTH;
    unsigned int* in = new unsigned int[arrayLength];
    unsigned int* out = new unsigned int[arrayLength];

    for (int i = 0; i<arrayLength; i++) {
        in[i] = rand();
    }

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

    radixSort(in,out,maskSize);

    //end measurements
    #ifdef TIME
        auto end = Clock::now();
                        file << LENGTH << " " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count()  << endl;
    #endif
    #ifdef LINUX
        /* Stop counters and store results in values */
                    retval = PAPI_stop_counters(values,NUM_EVENTS);
                    file << LENGTH << " " << values[0]  << endl;
    #endif

    //print a value to make sure it doesn't get removed by optimizations
    cout << out[LENGTH/2] << endl;

    delete in;
    delete out;
}

int main(int argc, const char* argv[]) {
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
    fstream outputFile;
    outputFile.open(tab2, ios::out);
    srand((unsigned) time(NULL)); //init seed


    for (int i = 0; i < 26; i++) {
        int LENGTH = pow(2,i);
        //run these one at a time...
        cout << "n: " << LENGTH << endl;
//        testRadix(LENGTH, 1, outputFile);
//        testRadix(LENGTH, 2, outputFile);
//        testRadix(LENGTH, 4, outputFile);
//        testRadix(LENGTH, 8, outputFile);
//        testRadix(LENGTH, 16, outputFile);
    }
    outputFile.close();

}