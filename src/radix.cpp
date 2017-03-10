#include <cstdlib>			//needed for random numbers
#include <iostream>
#include <ctime>
#include <cmath>			//library for math stuff
#include <fstream>
#include <cstring>

using namespace std;

const unsigned char INTSIZE = 32;
unsigned int arrayLength;

unsigned int getKey(unsigned int value, unsigned int offset, unsigned int mask) {
    return (value >> offset) & mask;
}

void countingSort(unsigned int array[], unsigned int out[], unsigned int mask, unsigned int offset) {
    cout << "bla"<< endl;
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

int main(int argc, const char* argv[]) {
    srand((unsigned) time(NULL)); //init seed
    arrayLength = 10;
    unsigned int* in = new unsigned int[arrayLength];
    unsigned int* out = new unsigned int[arrayLength];
    cout << "input:" << endl;
    for (int i = 0; i<arrayLength; i++) {
        in[i] = rand() % 4294967296; //integer max value+1
        cout << in[i] << " ";
    }
    cout << endl;
    radixSort(in,out,8);
    cout << "output:" << endl;
    for (int i = 0; i<arrayLength; i++) {
        cout << out[i] << " ";
    }
    cout << endl;
    delete in;
    delete out;

}