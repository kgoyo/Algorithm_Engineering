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
            int tmp = in->getValue(i,j);
            out->setValue(i,j,in->getValue(j,i));
            out->setValue(j,i,tmp);
        }
    }
}

//transposeOblivious inspired by
//http://users.cecs.anu.edu.au/~Alistair.Rendell/papers/coa.pdf

void obliviousRecurse(Matrix* in, Matrix* out, int lowx, int dx, int lowy, int dy) {
    cout << ".";
    if (dx>0 || dy>0) { //todo find out if recurse or base case should be default predicted branch
        //recurse
        if (dx > dy) {
            //split vertical
            obliviousRecurse(in, out, lowx, dx/2, lowy, dy);
            obliviousRecurse(in, out, lowx + dx/2 +1, dx/2, lowy, dy);
        } else {
            //split horizontal
            obliviousRecurse(in, out, lowx, dx, lowy, dy/2);
            obliviousRecurse(in, out, lowx, dx, lowy + dy/2 + 1, dy/2);
        }
    } else  {
        //base
        out->setValue(lowy,lowx,in->getValue(lowx,lowy)); //todo test if swapping x and y is better
    }
}

//precondition matrix must have size power of 2, and in and out must have same size
void transposeOblivious(Matrix* in, Matrix* out) {
    int size = in->size;
    obliviousRecurse(in, out, 0, size-1, 0, size-1);
    out->print("out:");
    cout << "transpose complete" << endl;
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
    Matrix* A = generateMatrix(4,20);
    A->print("A:");
    Matrix* B = generateMatrix(4,20);
    B->print("B:");
    Matrix* Bi = new Matrix(B->size);
    Matrix* Bo = new Matrix(B->size);
    Bi->zeroInit();
    Bo->zeroInit();
    transposeIgnorant(B,Bi);
    Bi->print("Bi:");
    transposeOblivious(B,Bo);
    Bo->print("Bo:");
    Matrix* C1 = simpleMult(A,B);
    C1->print("simpleMult:");
    Matrix* C2 = rowMult(A,Bi);
    C2->print("rowMult:");
    Matrix* D1 = simpleMultPlus(A,B);
    D1->print("simpleMultPlus:");
    Matrix* D2 = rowMultPlus(A,Bi);
    D2->print("rowMultPlus:");


    //delete matrices.....
}

