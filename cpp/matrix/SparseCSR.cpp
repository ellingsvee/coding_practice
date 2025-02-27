#include "SparseCSR.h"
#include <unordered_map>
#include <cassert>
#include <thread>

SparseCSR::SparseCSR(int rows, int cols) : rows(rows), cols(cols)
{
    rowPointers.resize(rows + 1, 0);
}

void SparseCSR::addValue(int row, int col, double value)
{
    if (value != 0.0)
    {
        tempData[row][col] += value;
    }

    rowPointers[row + 1]++;
}

void SparseCSR::finalize()
{
    rowPointers[0] = 0;
    for (int i = 0; i < rows; ++i)
    {
        rowPointers[i + 1] += rowPointers[i];
    }

    values.reserve(rowPointers.back());
    colIndices.reserve(rowPointers.back());

    for (int i = 0; i < rows; ++i)
    {
        for (const auto &[col, val] : tempData[i])
        {
            values.push_back(val);
            colIndices.push_back(col);
        }
    }
}

SparseCSR SparseCSR::multiply(const SparseCSR &other) const
{
    assert(cols == other.rows && "Matrix dimensions must match for multiplication.");

    SparseCSR result(rows, other.cols);

    std::vector<std::unordered_map<int, double>> tempResult(rows);

    for (int i = 0; i < rows; ++i)
    {
        for (int j = rowPointers[i]; j < rowPointers[i + 1]; ++j)
        {
            int colA = colIndices[j];
            double valA = values[j];

            for (int k = other.rowPointers[colA]; k < other.rowPointers[colA + 1]; ++k)
            {
                int colB = other.colIndices[k];
                double valB = other.values[k];

                tempResult[i][colB] += valA * valB;
            }
        }
    }

    for (int i = 0; i < rows; ++i)
    {
        for (const auto &[col, val] : tempResult[i])
        {
            result.addValue(i, col, val);
        }
    }

    result.finalize();
    return result;
}

SparseCSR SparseCSR::multiplyMultithread(const SparseCSR &other) const
{
    assert(cols == other.rows && "Matrix dimensions must match for multiplication.");

    SparseCSR result(rows, other.cols);

    // Set up the threads
    int numThreads = std::thread::hardware_concurrency();
    std::vector<std::thread> threads;
    int rowsPerThread = rows / numThreads;

    // Lambda function to handle row processing in parallel
    auto processRows = [&](int threadId)
    {
        int startRow = threadId * rowsPerThread;
        int endRow = (threadId == numThreads - 1) ? rows : startRow + rowsPerThread;

        for (int i = startRow; i < endRow; ++i)
        {
            for (int j = 0; j < other.cols; ++j)
            {
                double sum = 0.0;
                for (int k = rowPointers[i]; k < rowPointers[i + 1]; ++k)
                {
                    int colA = colIndices[k];
                    double valA = values[k];

                    for (int l = other.rowPointers[colA]; l < other.rowPointers[colA + 1]; ++l)
                    {
                        int colB = other.colIndices[l];
                        double valB = other.values[l];
                        if (colB == j)
                        {
                            sum += valA * valB;
                        }
                    }
                }
                if (sum != 0.0)
                {
                    result.addValue(i, j, sum);
                }
            }
        }
    };

    // Launch threads
    for (int i = 0; i < numThreads; i++)
    {
        threads.push_back(std::thread(processRows, i));
    }
    // Wait for all threads to finish
    for (auto &t : threads)
    {
        t.join();
    }

    result.finalize();
    return result;
}

void SparseCSR::print() const
{
    std::cout << "Values: ";
    for (double v : values)
        std::cout << v << " ";
    std::cout << "\nColumns: ";
    for (int c : colIndices)
        std::cout << c << " ";
    std::cout << "\nRow Pointers: ";
    for (int r : rowPointers)
        std::cout << r << " ";
    std::cout << "\n";
}

void SparseCSR::printDense() const
{
    std::vector<std::vector<double>> dense(rows, std::vector<double>(cols, 0.0));

    for (int row = 0; row < rows; row++)
    {
        for (int i = rowPointers[row]; i < rowPointers[row + 1]; i++)
        {
            int col = colIndices[i];
            dense[row][col] = values[i];
        }
    }

    for (const auto &row : dense)
    {
        for (const auto &val : row)
        {
            std::cout << val << "\t";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}