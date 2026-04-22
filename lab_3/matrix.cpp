#include "matrix.h"
#include <iostream>

matrix::matrix() : _rows(0), _cols(0), _data() {}

matrix::matrix(size_t rows, size_t cols) : _rows(rows), _cols(cols), _data(rows, std::vector<double>(cols, 0.0)) {}

matrix::matrix(size_t rows, size_t cols, std::vector<std::vector<double>> data) 
    : _rows(rows), _cols(cols), _data(rows, std::vector<double>(cols, 0.0)) {
    
    if (data.size() != rows) {
        throw std::invalid_argument("Number of rows in data does not match rows parameter");
    }

    for (size_t i = 0; i < rows; ++i) {
        if (data[i].size() != cols) {
            throw std::invalid_argument("Number of columns in data does not match cols parameter");
        }
    }
    
    _data = data;
}

size_t matrix::getRows() const {
    return _rows;
}

size_t matrix::getCols() const {
    return _cols;
}

void matrix::setElement(size_t i, size_t j, double value) {
    if (i >= _rows || j >= _cols) {
        throw std::out_of_range("Index out of bounds");
    }
    _data[i][j] = value;
}

double matrix::getElement(size_t i, size_t j) const {
    if (i >= _rows || j >= _cols) {
        throw std::out_of_range("Index out of bounds");
    }
    return _data[i][j];
}

void matrix::fill(double value) {
    for (size_t i = 0; i < _rows; ++i) {
        for (size_t j = 0; j < _cols; ++j) {
            _data[i][j] = value;
        }
    }
}