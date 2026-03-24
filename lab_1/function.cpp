#include <fstream>
#include <iostream>
#include <chrono>
#include <windows.h>
#include <iomanip>
#include <cstdlib>
#include <filesystem>
#include <random>
#include "function.h"
#include "globals.h"
                           
namespace fs = std::filesystem;

matrix matrix_multiplication(const matrix &matrix1, const matrix &matrix2){ //умножение матриц
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

matrix multiplication_square_matrices(const matrix &matrix1, const matrix &matrix2){ // умножение квадратных матриц
    if (matrix1.getRows()!=matrix1.getCols()||matrix2.getRows()!=matrix2.getCols()){
        throw std::invalid_argument("Matrices are not square");
    }
    return matrix_multiplication(matrix1, matrix2);
}

double measure_time(const matrix& matr1, const matrix& matr2, matrix& result) {//подсчет времени
    auto start = std::chrono::high_resolution_clock::now();
    
    result = multiplication_square_matrices(matr1, matr2);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    return duration.count() / 1000000.0; 
}

matrix read_matrix_from_file(const std::string& filename) {//считывание матрицы из файла
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

void print_matrix(const matrix& matr){// вывод матрицы в терминал
    for (size_t i = 0; i < matr.getRows(); ++i) {
        for (size_t j = 0; j < matr.getCols(); ++j) {  
            std::cout << matr.getElement(i, j) << " ";
        }
        std::cout << std::endl;
    }
}

void write_matrix_to_file(const matrix& matr, const std::string& filename) {//запись матрицы в файл
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

void write_matrix_to_file(const matrix& matr, const std::string& filename, size_t numberOperation, double time) {//запись матрицы в файл
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

int verificationOnPython(std::string matrix1_file,std::string matrix2_file, std::string result_file){//проверка результатов на Python
    std::cout << "\nRunning Python verification..." << std::endl;
    std::string python_script = BASEPATH+"verification.py";
    std::string python_cmd = "python \"" + python_script + "\" \"" + 
                         matrix1_file + "\" \"" + 
                         matrix2_file + "\" \"" + 
                         result_file +"\"";
    int verif = system(python_cmd.c_str());
    return verif;
}

double randomElement(){
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<double> dist(-1000,1000);
    return dist(gen);
}

matrix randomMatrix(const size_t rows, const size_t cols){
    if(rows == 0||cols== 0){
        std::cerr << "Warning: Creating empty matrix" << std::endl;
        return matrix();
    }
    matrix randMatr(rows, cols);
    for(size_t i = 0; i<rows; ++i){
        for (size_t j = 0; j<cols; ++j){
            randMatr.setElement(i,j,randomElement());
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

    write_matrix_to_file(randomMatrix(s_200,s_200), MATRIX[0]);
    write_matrix_to_file(randomMatrix(s_200,s_200), MATRIX[1]);
    write_matrix_to_file(randomMatrix(s_800,s_800), MATRIX[2]);
    write_matrix_to_file(randomMatrix(s_800,s_800), MATRIX[3]);
    write_matrix_to_file(randomMatrix(s_1200,s_1200), MATRIX[4]);
    write_matrix_to_file(randomMatrix(s_1200,s_1200), MATRIX[5]);
    write_matrix_to_file(randomMatrix(s_1600,s_1600), MATRIX[6]);
    write_matrix_to_file(randomMatrix(s_1600,s_1600), MATRIX[7]);
    write_matrix_to_file(randomMatrix(s_2000,s_2000), MATRIX[8]);
    write_matrix_to_file(randomMatrix(s_2000,s_2000), MATRIX[9]);
}

bool creatCSVFile(std::string size, size_t numberOperation, double time, std::string csvPath){

    std::cout << "\nRunning Python verification..." << std::endl;
    std::string python_script = BASEPATH+"dataProcessing.py";
    std::string python_cmd = "python \"" + python_script + "\" \"" + 
                        size + "\" \"" +
                        std::to_string(numberOperation) + "\" \"" + 
                        std::to_string(time) + "\" \"" + 
                        csvPath +"\"";
    int verif = system(python_cmd.c_str());
    return verif;
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

void experiment(std::string matrix1_file,std::string matrix2_file){
    try {
        matrix matr1 = read_matrix_from_file(matrix1_file);
        matrix matr2 = read_matrix_from_file(matrix2_file);
        matrix result;
    
        double time = measure_time(matr1, matr2, result);
        size_t numberOperation = matr1.getRows()* (2*matr1.getCols() -1)* matr2.getCols(); 
        
        std::string size = std::to_string(matr1.getRows());
        std::string result_file = BASEPATH+"matrix/result_"+size+".txt";
        write_matrix_to_file(result, result_file, numberOperation, time);
        std::cout << "\nResult saved to result.txt\n";
    
        int verif = verificationOnPython(matrix1_file, matrix2_file, result_file);
        if (verif == 0) {
            std::cout << "Verification successful!" << std::endl;
        } 
        else {
            std::cout << "Verification failed!" << std::endl;
        } 
        if(creatCSVFile(size, numberOperation, time, BASEPATH+"report/data.csv")){
            std::cout << "Data added successfully!" << std::endl;
        }
        else{
            std::cout << "Error adding data!" << std::endl;
        }
                      
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }

}
