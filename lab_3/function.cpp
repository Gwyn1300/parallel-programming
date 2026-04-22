#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <cstdlib>
#include <filesystem>
#include <random>
#include <mpi.h>
#include "function.h"
#include "globals.h"

namespace fs = std::filesystem;

matrix matrix_multiplication(const matrix &matrix1, const matrix &matrix2){
    if (!(matrix1.getCols() == matrix2.getRows())){
        throw std::invalid_argument("Matrices are not suitable for multiplication");
    }
    
    size_t rows1 = matrix1.getRows();
    size_t cols1_rows2 = matrix1.getCols();
    size_t cols2 = matrix2.getCols();
    matrix result = matrix(rows1, cols2);
    
    for (size_t i = 0; i < rows1; ++i){
        for (size_t j = 0; j < cols2; ++j){
            double value = 0;
            for (size_t k = 0; k < cols1_rows2; ++k){
                value += matrix1.getElement(i, k) * matrix2.getElement(k, j);
            }
            result.setElement(i, j, value);
        }
    }

    return result;
}

matrix multiplication_square_matrices(const matrix &matrix1, const matrix &matrix2){
    if (matrix1.getRows()!=matrix1.getCols()||matrix2.getRows()!=matrix2.getCols()){
        throw std::invalid_argument("Matrices are not square");
    }
    return matrix_multiplication(matrix1, matrix2);
}

matrix mpi_matrix_multiplication(const matrix &matrix1, const matrix &matrix2, int num_processes) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    size_t N = matrix1.getRows();
    size_t M = matrix2.getCols();
    size_t K = matrix1.getCols();
    
    if (N != matrix2.getRows()) {
        throw std::invalid_argument("Matrix dimensions mismatch for multiplication");
    }
    
    matrix result(N, M);
    
    int rows_per_process = static_cast<int>(N) / size;
    int remainder = static_cast<int>(N) % size;
    
    std::vector<int> send_counts(size);
    std::vector<int> displs(size);
    std::vector<int> recv_counts(size);
    std::vector<int> recv_displs(size);
    
    for (int i = 0; i < size; i++) {
        int rows_i = (i < remainder) ? rows_per_process + 1 : rows_per_process;
        send_counts[i] = rows_i * static_cast<int>(K);
        displs[i] = (i == 0) ? 0 : displs[i-1] + send_counts[i-1];
        recv_counts[i] = rows_i * static_cast<int>(M);
        recv_displs[i] = (i == 0) ? 0 : recv_displs[i-1] + recv_counts[i-1];
    }
    
    if (rank == 0) {
        std::vector<double> flat_matrix1(N * K);
        std::vector<double> flat_matrix2(K * M);
        
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < K; j++) {
                flat_matrix1[i * K + j] = matrix1.getElement(i, j);
            }
        }
        
        for (size_t i = 0; i < K; i++) {
            for (size_t j = 0; j < M; j++) {
                flat_matrix2[i * M + j] = matrix2.getElement(i, j);
            }
        }
        
        MPI_Bcast(flat_matrix2.data(), static_cast<int>(K * M), MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        for (int i = 1; i < size; i++) {
            MPI_Send(flat_matrix1.data() + displs[i], send_counts[i], MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
        
        int local_rows = (0 < remainder) ? rows_per_process + 1 : rows_per_process;
        std::vector<double> local_result(local_rows * M, 0.0);
        
        for (int i = 0; i < local_rows; i++) {
            int global_row = i;
            for (size_t j = 0; j < M; j++) {
                double sum = 0.0;
                for (size_t k = 0; k < K; k++) {
                    sum += flat_matrix1[global_row * K + k] * flat_matrix2[k * M + j];
                }
                local_result[i * M + j] = sum;
            }
        }
        
        std::vector<double> all_results(N * M);
        
        for (int i = 0; i < local_rows; i++) {
            for (size_t j = 0; j < M; j++) {
                all_results[i * M + j] = local_result[i * M + j];
            }
        }
        
        for (int i = 1; i < size; i++) {
            int rows_i = (i < remainder) ? rows_per_process + 1 : rows_per_process;
            std::vector<double> temp_result(rows_i * M);
            MPI_Recv(temp_result.data(), rows_i * static_cast<int>(M), MPI_DOUBLE, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            for (int j = 0; j < rows_i; j++) {
                for (size_t k = 0; k < M; k++) {
                    all_results[(recv_displs[i] / static_cast<int>(M) + j) * M + k] = temp_result[j * M + k];
                }
            }
        }
        
        for (size_t i = 0; i < N; i++) {
            for (size_t j = 0; j < M; j++) {
                result.setElement(i, j, all_results[i * M + j]);
            }
        }
        
    } else {
        std::vector<double> flat_matrix2(K * M);
        MPI_Bcast(flat_matrix2.data(), static_cast<int>(K * M), MPI_DOUBLE, 0, MPI_COMM_WORLD);
        
        int local_rows = (rank < remainder) ? rows_per_process + 1 : rows_per_process;
        std::vector<double> local_matrix1(local_rows * K);
        MPI_Recv(local_matrix1.data(), local_rows * static_cast<int>(K), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        std::vector<double> local_result(local_rows * M, 0.0);
        for (int i = 0; i < local_rows; i++) {
            for (size_t j = 0; j < M; j++) {
                double sum = 0.0;
                for (size_t k = 0; k < K; k++) {
                    sum += local_matrix1[i * K + k] * flat_matrix2[k * M + j];
                }
                local_result[i * M + j] = sum;
            }
        }
        
        MPI_Send(local_result.data(), local_rows * static_cast<int>(M), MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    }
    
    return result;
}

double measure_time(const matrix& matr1, const matrix& matr2, matrix& result, int num_processes) {
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    double start_time = MPI_Wtime();
    
    if (num_processes > 1) {
        result = mpi_matrix_multiplication(matr1, matr2, num_processes);
    } else {
        result = multiplication_square_matrices(matr1, matr2);
    }
    
    double end_time = MPI_Wtime();
    
    return end_time - start_time;
}

matrix read_matrix_from_file(const std::string& filename){
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    size_t rows;
    size_t cols;
    file >> rows >> cols;
    
    if (rows == 0 || cols == 0 || file.fail()) {
        throw std::runtime_error("Invalid matrix size in file: " + filename);
    }

    std::vector<std::vector<double>> data(rows, std::vector<double>(cols, 0.0f));
    
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            if (!(file >> data[i][j])) {
                throw std::runtime_error("Error reading matrix data from file: " + filename);
            }
        }
    }
    
    file.close();
    return matrix(rows, cols, data);
}

void print_matrix(const matrix& matr){
    for (size_t i = 0; i < matr.getRows(); ++i) {
        for (size_t j = 0; j < matr.getCols(); ++j) {  
            std::cout << matr.getElement(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

void write_matrix_to_file(const matrix& matr, const std::string& filename){
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to create file: " + filename);
    }

    file << matr.getRows() << ' ' << matr.getCols() << std::endl;
    for (size_t i = 0; i < matr.getRows(); ++i) {
        for (size_t j = 0; j < matr.getCols(); ++j) {  
            file << matr.getElement(i, j) << " ";
        }
        file << std::endl;
    }

    file.close();
}

void write_matrix_to_file(const matrix& matr, const std::string& filename, size_t numberOperation, double time){
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to create file: " + filename);
    }

    file << matr.getRows() << ' ' << matr.getCols() << std::endl;
    for (size_t i = 0; i < matr.getRows(); ++i) {
        for (size_t j = 0; j < matr.getCols(); ++j) {  
            file << std::fixed << std::setprecision(12);
            file << matr.getElement(i, j) << " ";
        }
        file << std::endl;
    }

    file << "Total operations:~" << numberOperation << std::endl;
    file << std::fixed << std::setprecision(12);
    file << "Execution time: " << time << " seconds\n";
    
    file.close();
}

int verificationOnPython(std::string matrix1_file, std::string matrix2_file, std::string result_file){
    std::cout << "\nRunning Python verification..." << std::endl;
    std::string python_script = BASEPATH + "verification.py";
    
    // Для Windows используем исправленные пути
    std::string python_cmd = "python \"" + python_script + "\" \"" + 
                             matrix1_file + "\" \"" + 
                             matrix2_file + "\" \"" + 
                             result_file + "\"";
    
    int verif = system(python_cmd.c_str());
    return verif;
}

double randomElement(){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> dist(-1000, 1000);
    return dist(gen);
}

matrix randomMatrix(const size_t rows, const size_t cols){
    if (rows == 0 || cols == 0) {
        std::cerr << "Warning: Creating empty matrix" << std::endl;
        return matrix();
    }
    matrix randMatr(rows, cols);
    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            randMatr.setElement(i, j, randomElement());
        }
    }
    return randMatr;
}

void creatMatrixForExperement(){
    size_t s_200 = 200;
    size_t s_800 = 800;
    size_t s_1200 = 1200;
    size_t s_1600 = 1600;
    size_t s_2000 = 2000;

    write_matrix_to_file(randomMatrix(s_200, s_200), MATRIX[0]);
    write_matrix_to_file(randomMatrix(s_200, s_200), MATRIX[1]);
    write_matrix_to_file(randomMatrix(s_800, s_800), MATRIX[2]);
    write_matrix_to_file(randomMatrix(s_800, s_800), MATRIX[3]);
    write_matrix_to_file(randomMatrix(s_1200, s_1200), MATRIX[4]);
    write_matrix_to_file(randomMatrix(s_1200, s_1200), MATRIX[5]);
    write_matrix_to_file(randomMatrix(s_1600, s_1600), MATRIX[6]);
    write_matrix_to_file(randomMatrix(s_1600, s_1600), MATRIX[7]);
    write_matrix_to_file(randomMatrix(s_2000, s_2000), MATRIX[8]);
    write_matrix_to_file(randomMatrix(s_2000, s_2000), MATRIX[9]);
}

bool creatCSVFile(std::string size, size_t numberOperation, double time, int num_processes, std::string csvPath){
    std::cout << "\nRunning Python data processing..." << std::endl;
    std::string python_script = BASEPATH + "dataProcessing.py";
    std::string python_cmd = "python \"" + python_script + "\" \"" + 
                             size + "\" \"" +
                             std::to_string(numberOperation) + "\" \"" + 
                             std::to_string(time) + "\" \"" + 
                             std::to_string(num_processes) + "\" \"" +
                             csvPath + "\"";
    
    int verif = system(python_cmd.c_str());
    return (verif == 0);
}

bool createGraphsFromCSV(std::string csv_path) {
    std::cout << "\nCreating graphs from CSV..." << std::endl;
    std::string python_script = BASEPATH + "dataVisualization.py";
    std::string output_dir = REPORTPATH;
    
    std::string python_cmd = "python \"" + python_script + "\" \"" + 
                             csv_path + "\" \"" + 
                             output_dir + "\"";
    
    std::cout << "Command: " << python_cmd << std::endl;
    
    int result = system(python_cmd.c_str());
    return (result == 0);
}

void experiment(std::string matrix1_file, std::string matrix2_file, int num_processes){
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    try {
        matrix matr1 = read_matrix_from_file(matrix1_file);
        matrix matr2 = read_matrix_from_file(matrix2_file);
        matrix result;
        
        double time = measure_time(matr1, matr2, result, num_processes);
        size_t numberOperation = matr1.getRows() * (2 * matr1.getCols() - 1) * matr2.getCols();
        
        if (rank == 0) {
            std::string size = std::to_string(matr1.getRows());
            std::string result_file = BASEPATH + "matrix/result_" + size + "_mpi_" + std::to_string(num_processes) + ".txt";
            write_matrix_to_file(result, result_file, numberOperation, time);
            std::cout << "\nResult saved to " << result_file << std::endl;
            
            int verif = verificationOnPython(matrix1_file, matrix2_file, result_file);
            if (verif == 0) {
                std::cout << "Verification successful!" << std::endl;
            }
            else {
                std::cout << "Verification failed!" << std::endl;
            }
            
            if (creatCSVFile(size, numberOperation, time, num_processes, BASEPATH + "report/data_mpi.csv")) {
                std::cout << "Data added successfully!" << std::endl;
            }
            else {
                std::cout << "Error adding data!" << std::endl;
            }
        }
        
    }
    catch (const std::exception& e) {
        if (rank == 0) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
        return;
    }
}
