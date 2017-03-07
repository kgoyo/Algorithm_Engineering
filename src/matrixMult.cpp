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
        height = size;
        width = size;
        p = new int[n*n];
    }

    Matrix(int h, int w) {
        height = h;
        width = w;
        p = new int[h*w];
    }

    int* p;
    int size;
    int height;
    int width;

    int getValue(int row, int col){
        return *(p + row * height + col);
    }

    void setValue(int row, int col, int value) {
        p[row * height + col] = value;
    }

    void addValue(int row, int col, int value) {
        p[row * height + col] += value;
    }

    void print() {
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                cout << getValue(i,j) << " ";
            }
            cout << endl;
        }
    }

    void printOrder() {
        for (int i =0; i< height*width; i++) {
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

int findMax(int a, int b, int c) {
    return max(a,max(b,c));
}

/*
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

    }
}*/

Matrix* joinHorizontal(Matrix* A, Matrix* B) {
    Matrix* C = new Matrix(A->height +  B->height, A->width);

    //populate upper part
    for (int i = 0; i < C->width; i++) {
        for (int j = 0; j < A->height; j++) {
            C->setValue(i,j,A->getValue(i,j));
        }
    }
    //populate lower part
    for (int i = 0; i < C->width; i++) {
        for (int j = 0; j < B->height; j++) {
            C->setValue(i+A->height,j,B->getValue(i,j));
        }
    }

    cout << "join horizontal:" << endl << "input:" << endl;
    A->print();
    B->print();
    cout << "output:" << endl;
    C->print();

    return C;
}

Matrix* joinVertical(Matrix* A, Matrix* B) {
    Matrix* C = new Matrix(A->height, A->width + B->width);

    //populate left part
    for (int i = 0; i < A->width; i++) {
        for (int j = 0; j < C->height; j++) {
            C->setValue(i,j,A->getValue(i,j));
        }
    }
    //populate right part
    for (int i = 0; i < B->width; i++) {
        for (int j = 0; j < C->height; j++) {
            C->setValue(i,j + A->width,B->getValue(i,j));
        }
    }
    cout << "join vertical:" << endl << "input:" << endl;
    A->print();
    B->print();
    cout << "output:" << endl;
    C->print();

    return C;
}
//todo fix offsets to (r,c) from (x,y)
Matrix* multRecurse(Matrix* A, Matrix* B, int n, int m, int p) {
    int max = findMax(n, m, p);
    Matrix* res;
    if (max==1) {
        //base case
        cout << "0:" << endl;
        res = new Matrix(1);
        int value = A->getValue(0,0) * B->getValue(0,0);
        res->setValue(0,0,value);
    } else if (max==n) {
        cout << "n:" << n << endl;
        //split A horizontally
        int splitHeight = (A->height)/2;
        int width = A->width;

        Matrix* A1 = new Matrix(splitHeight, width);
        Matrix* A2 = new Matrix((A->height)-splitHeight, width);

        //todo try reverse order of for loops
        //populate upper matrix
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < splitHeight; j++) {
                A1->setValue(i,j,A->getValue(i,j));
            }
        }
        //todo try reverse order of for loops
        //populate lower matrix
        for (int i = 0; i < width; i++) {
            for (int j = splitHeight; j < A->height; j++) {
                A2->setValue(i,j-splitHeight,A->getValue(i,j));
            }
        }
        Matrix* C1 = multRecurse(A1, B, n/2,m,p);
        Matrix* C2 = multRecurse(A2, B, n-(n/2),m,p);
        res = joinHorizontal(C1, C2);
    } else if (max==p) {
        cout << "p:" << p << endl;
        //split B vertically
        int splitWidth = (B->width)/2;
        int height = B->height;

        Matrix* B1 = new Matrix(height, splitWidth);
        Matrix* B2 = new Matrix(height, (B->width)-splitWidth);

        //todo try reverse order of for loops
        //populate left matrix
        for (int i = 0; i < splitWidth; i++) {
            for (int j = 0; j < height; j++) {
                B1->setValue(i,j,B->getValue(i,j));
            }
        }
        //todo try reverse order of for loops
        //populate right matrix
        for (int i = splitWidth; i < B->width; i++) {
            for (int j = 0; j < height; j++) {
                B2->setValue(i-splitWidth,j,B->getValue(i,j));
            }
        }
        Matrix* C1 = multRecurse(A, B1, n, m, p/2);
        Matrix* C2 = multRecurse(A, B2, n, m, p-(p/2));
        res = joinVertical(C1, C2);

    } else { //max==m
        cout << "m:" << m << endl;
        //split A vertically
        int splitWidth = (A->width)/2;
        Matrix* A1 = new Matrix(A->height, splitWidth);
        Matrix* A2 = new Matrix(A->height, (A->width)-splitWidth);

        //todo try reverse order of for loops
        //populate left matrix
        for (int i = 0; i < splitWidth; i++) {
            for (int j = 0; j < A->height; j++) {
                A1->setValue(i,j,A->getValue(i,j));
            }
        }
        //todo try reverse order of for loops
        //populate right matrix
        for (int i = splitWidth; i < A->width; i++) {
            for (int j = 0; j < A->height; j++) {
                A2->setValue(i-splitWidth,j,A->getValue(i,j));
            }
        }

        //split B horizontally
        int splitHeight = (B->height)/2;
        Matrix* B1 = new Matrix(splitHeight, B->width);
        Matrix* B2 = new Matrix((B->height)-splitHeight, B->width);

        //todo try reverse order of for loops
        //populate upper matrix
        for (int i = 0; i < B->width; i++) {
            for (int j = 0; j < splitHeight; j++) {
                B1->setValue(i,j,B->getValue(i,j));
            }
        }
        //todo try reverse order of for loops
        //populate lower matrix
        for (int i = 0; i < B->width; i++) {
            for (int j = splitHeight; j < A->height; j++) {
                B2->setValue(i,j-splitHeight,B->getValue(i,j));
            }
        }

        Matrix* C1 = multRecurse(A1, B1, n, m/2, p);
        Matrix* C2 = multRecurse(A2, B2, n, m-(m/2),p);
        res = new Matrix(C1->height, C1->width);

        //todo try reverse order of for loops
        for (int i = 0; i< res->width; i++) {
            for (int j = 0; i< res->height; i++) {
                int value = C1->getValue(i,j) + C2->getValue(i,j);
                res->setValue(i,j,value);
            }
        }
    }
    res->print();
    return res;
}

Matrix* recursiveMult(Matrix *A, Matrix *B) {
    int size = A->size;
    return multRecurse(A, B, size, size, size);
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
//    Matrix* D = recursiveMult(A,B);
//    cout << "D: " << endl;
//    D->print();
}

