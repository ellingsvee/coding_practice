#ifndef SPARSECSR_H
#define SPARSECSR_H

#include <vector>
#include <iostream>

class SparseCSR
{
private:
    int rows, cols;
    std::vector<double> values;
    std::vector<int> colIndices;
    std::vector<int> rowPointers;
    std::unordered_map<int, std::unordered_map<int, double>> tempData;

public:
    SparseCSR(int rows, int cols);
    void addValue(int row, int col, double value);
    void finalize();
    SparseCSR multiply(const SparseCSR &other) const;
    void print() const;
    void printDense() const;
};

#endif // SPARSECSR_H
