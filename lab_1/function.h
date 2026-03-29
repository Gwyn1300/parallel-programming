#include <vector>
#include<string>
#include"matrix.h"

matrix matrix_multiplication(const matrix &matrix1, const matrix &matrix2);

matrix matrix_multiplication_OMP(const matrix &matrix1, const matrix &matrix2, int num_threads);

matrix multiplication_square_matrices(const matrix &matrix1, const matrix &matrix2);

double measure_time(const matrix& matr1, const matrix& matr2, matrix& result);

matrix read_matrix_from_file(const std::string& filename);

void print_matrix(const matrix& matr);

void write_matrix_to_file(const matrix& matr, const std::string& filename);

void write_matrix_to_file(const matrix& matr, const std::string& filename, size_t numberOperation, double time) ;

int verificationOnPython(std::string matrix1_file,std::string matrix2_file, std::string result_file);

double randomElement();

matrix randomMatrix(const size_t rows, const size_t cols);

void creatMatrixForExperement();

void experiment(std::string matrix1_file,std::string matrix2_file, int num_threads);

bool creatCSVFile(std::string size, size_t numberOperation, double time, int num_threads, std::string csvPath);

bool createGraphsFromCSV(std::string csv_path);

void experiment(std::string matrix1_file,std::string matrix2_file, int num_threads);
