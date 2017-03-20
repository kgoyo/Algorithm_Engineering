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

typedef struct Matrix {

    Matrix(int n) {
        size = n;
        numIndices = n*n;
        p = new int[numIndices];
    }


    int* p;
    int size;
    int numIndices;

    int getValue(int row, int col){
        return *(p + row * size + col);
    }

    int * getPointer(int row, int col) {
        return p + row * size + col;
    }

    void setValue(int row, int col, int value) {
        p[row * size + col] = value;
    }

    void addValue(int row, int col, int value) {
        p[row * size + col] += value;
    }

    void zeroInit() {
        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                setValue(r,c,0);
            }
        }
    }

    void print(string title) {
        cout << title << endl;
        for (int r = 0; r < size; r++) {
            for (int c = 0; c < size; c++) {
                cout << getValue(r,c) << " ";
            }
            cout << endl;
        }
    }

    void printOrder() {
        for (int i =0; i< numIndices; i++) {
            cout << p[i] << endl;
        }
    }

    ~Matrix() {
        delete [] p;
    }

}Matrix;

Matrix* generateMatrix(int n, int maxint) {
    Matrix* mat = new Matrix(n);
    for(int i=0 ; i < n; i++){
        for(int j=0; j < n; j++) {
            int r = rand() % maxint;
            mat->setValue(i,j,r);
        }
    }
    return mat;
}

Matrix* simpleMult(Matrix* A, Matrix* B) {
    int size = A->size;
    Matrix* C = new Matrix(size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int res = 0;
            for (int k = 0; k < size; k++) {
                res+= A->getValue(i,k) * B->getValue(k,j);
            }
            C->setValue(i,j,res);
        }
    }
    return C;
}

//less multiplication, about 3 times faster
Matrix* simpleMultPlus(Matrix* A, Matrix* B, Matrix* C) {
    int size = A->size;
    int *p = C->p;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int res = 0;
            int *ap = A->getPointer(i,0);
            int *bp = B->getPointer(0,j);
            for (int k = 0; k < size; k++) {
                res+= (*ap) * (*bp);
                ap++;
                bp+=B->size;
            }
            *p = res;
            p++;
        }
    }
}

//transposeIgnorant inspired by
//http://users.cecs.anu.edu.au/~Alistair.Rendell/papers/coa.pdf
//precondition in and out must have same size
void transposeIgnorant(Matrix* in, Matrix* out) {
    int n = in->size;
    for(int i=0 ; i < n; i++) { //we start from 0
        for (int j = 0; j <= i; j++) { //we do <= instead of <
            out->setValue(i,j,in->getValue(j,i));
            out->setValue(j,i,in->getValue(i,j));
        }
    }
}

//reads input row by row, and outputs column by column
//faster in practice
void transposeIgnorantPlus(Matrix* in, Matrix* out) {
    int * p = in->p;
    for (int * i = out->p; i < out->p + out->size; i++ ) {
        for (int * j = i; j < out->p + out->numIndices; j+=out->size) {
            *j = *p;
            p++;
        }
    }
}

//transposeOblivious inspired by
//http://users.cecs.anu.edu.au/~Alistair.Rendell/papers/coa.pdf
void obliviousRecurse(Matrix* in, Matrix* out, int lowr, int dr, int lowc, int dc) {
    if (dr>0 || dc>0) {
        //recurse
        if (dr > dc) {
            //split horizontal
            obliviousRecurse(in, out, lowr, dr/2, lowc, dc);
            obliviousRecurse(in, out, lowr + dr/2 +1, dr/2, lowc, dc);
        } else {
            //split vertical
            obliviousRecurse(in, out, lowr, dr, lowc, dc/2);
            obliviousRecurse(in, out, lowr, dr, lowc + dc/2 + 1, dc/2);
        }
    } else  {
        //base
        out->setValue(lowc,lowr,in->getValue(lowr,lowc));
    }
}

//precondition matrix must have size power of 2, and in and out must have same size
void transposeOblivious(Matrix* in, Matrix* out) {
    int size = in->size;
    obliviousRecurse(in, out, 0, size-1, 0, size-1);
}

void obliviousRecursePlus(int * in, int * out, int dc, int dr, int width) {
    if (dr>0 || dc>0) {
        //recurse
        if (dr > dc) {
            //split horizontal
            obliviousRecursePlus(in, out, dc, dr/2, width);
            obliviousRecursePlus(in + width * (dr/2+1), out + dr/2 + 1, dc, dr/2, width);
        } else {
            //split vertical
            obliviousRecursePlus(in, out, dc/2, dr, width);
            obliviousRecursePlus(in + dc/2 + 1, out + width * (dc/2+1), dc/2, dr, width);
        }
    } else  {
        //base
        *out = *in;
    }
}

void transposeObliviousPlus(Matrix* in, Matrix* out) {
    int size = in->size;
    obliviousRecursePlus(in->p, out->p, size-1, size-1, size);
}

//precondition B is already transposed
Matrix* rowMult(Matrix* A, Matrix* B) {
    int size = A->size;
    Matrix* C = new Matrix(size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int res = 0;
            for (int k = 0; k < size; k++) {
                res+= A->getValue(i,k) * B->getValue(j,k); //k and j is swapped from simpleMult
            }
            C->setValue(i,j,res);
        }
    }
    return C;
}

//less multiplication
//precondition B is already transposed
//result is put into C
Matrix* rowMultPlus(Matrix* A, Matrix* B, Matrix* C) {
    int size = A->size;
    int *p = C->p;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            int res = 0;
            int *ap = A->getPointer(i,0);
            int *bp = B->getPointer(j,0);
            for (int k = 0; k < size; k++) {
                res+= (*(ap+k)) * (*(bp+k));
            }
            *p = res;
            p++;
        }
    }
}

void testSimpleMult(int LENGTH, fstream& file) {
    //init
    Matrix* A = generateMatrix(LENGTH,10);
    Matrix* B = generateMatrix(LENGTH,10);
    Matrix* C = new Matrix(LENGTH);

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

    simpleMultPlus(A, B, C);

    //end measurements
    #ifdef TIME
        auto end = Clock::now();
            file << LENGTH << " " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << endl;
    #endif
    #ifdef LINUX
        /* Stop counters and store results in values */
        retval = PAPI_stop_counters(values,NUM_EVENTS);
        file << LENGTH << " " << values[0]  << endl;
    #endif

    //print a value in C to make sure it doesn't get removed by optimizations
    cout << C->getValue(0,0) << endl;

    delete A;
    delete B;
    delete C;
}

void testRowMult(int LENGTH, fstream& file) {
    //init
    Matrix* A = generateMatrix(LENGTH,10);
    Matrix* B = generateMatrix(LENGTH,10);
    Matrix* C = new Matrix(LENGTH);

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

    rowMultPlus(A, B, C);

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

    //print a value in C to make sure it doesn't get removed by optimizations
    cout << C->getValue(0,0) << endl;

    delete A;
    delete B;
    delete C;
}

void testIgnorantTranspose(int LENGTH, fstream& file) {
    //init
    Matrix* A = generateMatrix(LENGTH,10);
    Matrix* B = new Matrix(LENGTH);

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

    transposeIgnorantPlus(A, B);

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

    //print a value in C to make sure it doesn't get removed by optimizations
    cout << B->getValue(0,0) << endl;

    delete A;
    delete B;
}

void testObliviousTranspose(int LENGTH, fstream& file) {
    //init
    Matrix* A = generateMatrix(LENGTH,10);
    Matrix* B = new Matrix(LENGTH);

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

    transposeOblivious(A, B);

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

    //print a value in C to make sure it doesn't get removed by optimizations
    cout << B->getValue(0,0) << endl;

    delete A;
    delete B;
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

    srand((unsigned)time(NULL)); //init seed

    //test mult
    for (int i = 0; i <= 10 ;i++) {
        int LENGTH = pow(2,i);
        cout << "n: " << LENGTH << endl;
        //testSimpleMult(LENGTH, outputFile);
        //testRowMult(LENGTH, outputFile);
    }


    //test transpose
    for (int i = 0; i <= 15 ;i++) {
        int LENGTH = pow(2,i);
        cout << "n: " << LENGTH << endl;
        //testIgnorantTranspose(LENGTH, outputFile);
        //testObliviousTranspose(LENGTH, outputFile);
    }
    outputFile.close();
}

