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
        p = new int[n*n];
    }

    int* p;
    int size;

    int getValue(int row, int col){
        return *(p + row * size + col);
    }

    void setValue(int row, int col, int value) {
        p[row * size + col] = value;
    }

    void addValue(int row, int col, int value) {
        p[row * size + col] += value;
    }

    void print() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                cout << getValue(i,j) << " ";
            }
            cout << endl;
        }
    }

    void printOrder() {
        for (int i =0; i< size*size; i++) {
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

//    //check if same size
//    if (A->size != B->size) {
//        throw "input Matrices not same size";
//    }
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

//transposeIgnorant inspired by
//http://users.cecs.anu.edu.au/~Alistair.Rendell/papers/coa.pdf

void transposeIgnorant(Matrix* mat) {
    int n = mat->size;
    for(int i=1 ; i < n; i++) {
        for (int j = 0; j < i; j++) {
            int tmp = mat->getValue(i,j);
            mat->setValue(i,j,mat->getValue(j,i));
            mat->setValue(j,i,tmp);
        }
    }
}

//transposeOblivious inspired by
//http://users.cecs.anu.edu.au/~Alistair.Rendell/papers/coa.pdf

void obliviousRecurse(Matrix* in, Matrix* out, int lowx, int dx, int lowy, int dy) {
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

void transposeOblivious(Matrix*& mat) {
    int size = mat->size;
    Matrix* out = new Matrix(size);
    obliviousRecurse(mat, out, 0, size-1, 0, size-1);

    //do more stuff
    Matrix* temp2 = mat;
    mat = &(*out);
    delete temp2;
}

Matrix* rowMult(Matrix* A, Matrix* B) {
    transposeIgnorant(B);
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

//returns a value from 1 to 3 depeding on which input is largest
int findMax(int a, int b, int c) {
    return max(a,max(b,c));
}

void multRecurse(Matrix* A, Matrix* B, Matrix* C, int lowxA, int lowyA, int lowxB, int lowyB, int n, int m, int p) {
    //cout << "n: " << n << " m: " << m << " p: "<< p << endl;
    int max = findMax(n, m, p);
    if (max==0) {
        cout << "A: (" << lowxA << "," << lowyA << ") B: (" << lowxB << "," << lowyB << ")" << " res: (" << lowxA << "," << lowyB << ")" <<  endl;
        int res = A->getValue(lowxA, lowyA) * B->getValue(lowxB, lowyB);
        C->addValue(lowxA,lowyB,res);
    } else if (max==n) {
        cout << "n" << endl;
        //split A horizontally
        multRecurse(A,B,C,lowxA,lowyA,lowxB,lowyB,n/2,m,p);
        int lowyA2 = lowyA + n/2 +1;
        multRecurse(A,B,C,lowxA,lowyA2,lowxB,lowyB,n-(lowyA2-lowyA),m,p);
    } else if (max==p) {
        cout << "p" << endl;
        //split B vertically
        multRecurse(A,B,C,lowxA,lowyA,lowxB,lowyB,n,m,p/2);
        int lowxB2 = lowxB + p/2 +1;
        multRecurse(A,B,C,lowxA,lowyA,lowxB2,lowyB,n,m,p-(lowxB2-lowxB));
    } else { //max==m
        cout << "m" << endl;

        multRecurse(A,B,C,lowxA,lowyA,lowxB,lowyB,n,m/2,p);
        int lowxA2 = lowxA + m/2 +1;
        int lowyB2 = lowyB + m/2 +1;
        multRecurse(A,B,C,lowxA2,lowyA,lowxB,lowyB2,n,m-(lowxA2-lowxA),p);

        /*
        //split A vertically
        multRecurse(A,B,C,lowxA,lowyA,lowxB,lowyB,n,m/2,p);
        int lowxA2 = lowxA + m/2 +1;
        multRecurse(A,B,C,lowxA2,lowyA,lowxB,lowyB,n,m-(lowxA2-lowxA),p);
        //split B horizontally
        multRecurse(A,B,C,lowxA,lowyA,lowxB,lowyB,n,m/2,p);
        int lowyB2 = lowyB + m/2 +1;
        multRecurse(A,B,C,lowxA,lowyA,lowxB,lowyB2,n,m-(lowyB2-lowyB),p);
         */
    }
}


Matrix* blastProcessingMult(Matrix* A, Matrix* B) {
    int size = A->size;
    Matrix* C = new Matrix(size);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            C->setValue(i,j,0);
        }
    }
    multRecurse(A, B, C, 0, 0, 0, 0, size-1, size-1, size-1);
    return C;
}

int main(int argc, const char* argv[]) {
    srand((unsigned)time(NULL)); //init seed
    Matrix* A = generateMatrix(2,20);
    Matrix* B = generateMatrix(2,20);
    Matrix* C = simpleMult(A,B);
    cout << "A:" << endl;
    A->print();
    cout << "B:" << endl;
    B->print();
    cout << "C:" << endl;
    C->print();
//    cout << "C^T:" << endl;
//    transposeOblivious(C);
//    C->print();
    Matrix* D = blastProcessingMult(A,B);
    cout << "D: " << endl;
    D->print();
//
}

