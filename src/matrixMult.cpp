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

    void print() {
        cout << "lol" << endl;
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

int main(int argc, const char* argv[]) {
    srand((unsigned)time(NULL)); //init seed
    generateMatrix(10,20);
    cout << "done"<< endl;
}

