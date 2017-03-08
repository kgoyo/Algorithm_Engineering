#include <cstdlib>			//needed for random numbers
#include <iostream>
#include <ctime>
#include <cmath>			//library for math stuff
#include <fstream>
#include <cstring>

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
Matrix* simpleMultPlus(Matrix* A, Matrix* B) {
    int size = A->size;
    Matrix* C = new Matrix(size);
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
    return C;
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
    if (dr>0 || dc>0) { //todo find out if recurse or base case should be default predicted branch
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
        out->setValue(lowc,lowr,in->getValue(lowr,lowc)); //todo test if swapping x and y is better
    }
}

//precondition matrix must have size power of 2, and in and out must have same size
void transposeOblivious(Matrix* in, Matrix* out) {
    int size = in->size;
    obliviousRecurse(in, out, 0, size-1, 0, size-1);
}

void obliviousRecursePlus(int * in, int * out, int dc, int dr, int width, int* inMaxPointer, int* outMaxPointer) {
    if (in > inMaxPointer) {
        cout << "too far in" << endl;
    }
    if (out > outMaxPointer) {
        cout << "too far out" << endl;
    }
    if (dr>0 || dc>0) {
        //recurse
        if (dr > dc) {
            //split horizontal
//            obliviousRecurse(in, out, lowr, dr/2, lowc, dc);
            cout << "QWERT" << endl;
            obliviousRecursePlus(in, out, dc, dr/2, width, inMaxPointer, outMaxPointer);
//            obliviousRecurse(in, out, lowr + dr/2 +1, dr/2, lowc, dc);
            cout << "ASDFG" << endl;
            obliviousRecursePlus(in + width * (dr/2+1), out + dr/2 + 1, dc, dr/2, width, inMaxPointer, outMaxPointer);
        } else {
            //split vertical
//            obliviousRecurse(in, out, lowr, dr, lowc, dc/2);
            cout << "ZXCVBN" << endl;
            obliviousRecursePlus(in, out, dc/2, dr, width, inMaxPointer, outMaxPointer);
//            obliviousRecurse(in, out, lowr, dr, lowc + dc/2 + 1, dc/2);
            cout << "YYUIOP" << endl;
            obliviousRecursePlus(in + dc/2 + 1, out + width * (dc/2+1), dc/2, dr, width, inMaxPointer, outMaxPointer);
        }
    } else  {
        //base
        *out = *in;
    }
}

void transposeObliviousPlus(Matrix* in, Matrix* out) {
    int size = in->size;
    obliviousRecursePlus(in->p, out->p, size-1, size-1, size, in->p +in->numIndices, out->p + out->numIndices);
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
Matrix* rowMultPlus(Matrix* A, Matrix* B) {
    int size = A->size;
    Matrix* C = new Matrix(size);
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
    return C;
}

int main(int argc, const char* argv[]) {
    srand((unsigned)time(NULL)); //init seed
//    Matrix* A = generateMatrix(4,20);
//    A->print("A:");
//    Matrix* B = generateMatrix(4,20);
//    B->print("B:");
//    Matrix* Bi = new Matrix(B->size);
//    Matrix* Bo = new Matrix(B->size);
//    Bi->zeroInit();
//    Bo->zeroInit();
//    transposeIgnorant(B,Bi);
//    Bi->print("Bi:");
//    transposeOblivious(B,Bo);
//    Bo->print("Bo:");
//    Matrix* C1 = simpleMult(A,B);
//    C1->print("simpleMult:");
//    Matrix* C2 = rowMult(A,Bi);
//    C2->print("rowMult:");
//    Matrix* D1 = simpleMultPlus(A,B);
//    D1->print("simpleMultPlus:");
//    Matrix* D2 = rowMultPlus(A,Bi);
//    D2->print("rowMultPlus:");

    Matrix* E = generateMatrix(8192,20);
    cout << "done generating" << endl;
    Matrix* F = new Matrix(E->size);
    Matrix* G = new Matrix(E->size);
    Matrix* H = new Matrix(E->size);
    Matrix* I = new Matrix(E->size);
    clock_t c1 = clock();
//    transposeIgnorantPlus(E,F);
    clock_t c2 = clock();
//    transposeIgnorant(E,G);
    clock_t c3 = clock();
    transposeOblivious(E,H);
    clock_t c4 = clock();
    transposeObliviousPlus(E,I);
    clock_t c5 = clock();
    cout << "F:" << double(c2 - c1) / CLOCKS_PER_SEC << endl;
    cout << "G:" << double(c3 - c2) / CLOCKS_PER_SEC << endl;
    cout << "H:" << double(c4 - c3) / CLOCKS_PER_SEC << endl;
    cout << "I:" << double(c5 - c4) / CLOCKS_PER_SEC << endl;
//    cout << F->getValue(0,0) << G->getValue(0,0) << H->getValue(0,0);
    //delete matrices.....
}

