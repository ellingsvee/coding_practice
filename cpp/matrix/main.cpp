#include "SparseCSR.h"

// clang++ - std = c++ 17 - o main SparseCSR.cpp main.cpp

int main()
{
    SparseCSR A(3, 3);
    A.addValue(0, 0, 1);
    A.addValue(0, 1, 2);
    A.addValue(1, 1, 3);
    A.addValue(2, 2, 4);
    A.finalize();
    A.printDense();

    SparseCSR B(3, 3);
    B.addValue(0, 0, 5);
    B.addValue(1, 0, 6);
    B.addValue(1, 2, 7);
    B.addValue(2, 2, 8);
    B.finalize();
    B.printDense();

    SparseCSR C = A.multiply(B);
    C.printDense();

    return 0;
}
