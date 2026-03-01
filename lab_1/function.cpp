#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <windows.h>
#include <iomanip>
#include <cstdlib>
#include <filesystem>


namespace fs = std::filesystem;

class matrix{ 
    size_t _rows = 0;
    size_t _cols = 0;
    std::vector<std::vector<float>> _data;
    
    public:
    matrix():_rows(0), _cols(0), _data(){}

    matrix(size_t rows, size_t cols):_rows(rows), _cols(cols), _data(rows, std::vector<float>(cols, 0.0f)){
        for (size_t i = 0; i<_rows; ++i){
            _data[i].resize(cols, 0);
        } 
    }

    matrix(size_t rows, size_t cols, std::vector<std::vector<float>> data) : _rows(rows), _cols(cols), _data() {
        
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
    
    size_t getRows() const {
        return _rows;
    }
    
    size_t getCols() const {
        return _cols;
    }

    void setElement(size_t i, size_t j, float value) {
        if (i >= _rows || j >= _cols) {
            throw std::out_of_range("Index out of bounds");
        }
        _data[i][j] = value;
    }
    
    float getElement(size_t i, size_t j) const {
        if (i >= _rows || j >= _cols) {
            throw std::out_of_range("Index out of bounds");
        }
        return _data[i][j];
    }
    
    void fill(float value) {
        for (size_t i = 0; i < _rows; ++i) {
            for (size_t j = 0; j < _cols; ++j) {
                _data[i][j] = value;
            }
        }
    }
};

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
            float value = 0;
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

    std::vector<std::vector<float>> data(rows, std::vector<float>(cols, 0.0f));
    
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

void write_matrix_to_file(const matrix& matr, const std::string& filename, int numberOperation, double time) {//запись матрицы в файл
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

    file << "Total operations:~" << numberOperation << std::endl;
    file << std::fixed << std::setprecision(9);
    file << "Execution time: " << time << " seconds\n";

    file.close();
}

bool verificationOnPython(std::string matrix1_file,std::string matrix2_file, std::string result_file){//проверка результатов на Python
    std::cout << "\nRunning Python verification..." << std::endl;
    std::string base_path = "../../../Lab_1/";
    std::string python_script =base_path+"func.py";
    std::string python_cmd = "python \"" + python_script + "\" \"" + 
                         matrix1_file + "\" \"" + 
                         matrix2_file + "\" \"" + 
                         result_file +"\"";
    bool verif = system(python_cmd.c_str());
    return verif;
}

std::string readPath(){//считывание пути к файлу
    std::string pathToFile;
    std::getline(std::cin, pathToFile);
    if (!pathToFile.empty() && pathToFile.front() == '"' && pathToFile.back() == '"') {
        pathToFile = pathToFile.substr(1, pathToFile.size() - 2);
    }
    return pathToFile;
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    
    try {
        //считываем пути к матрицам
        std::cout << "Enter path to matrix one: ";
        std::string matrix1_file;
        matrix1_file = readPath();
        std::cout << "Enter path to matrix two: ";
        std::string matrix2_file;
        matrix2_file = readPath();

        //считываем матрицы
        matrix matr1 = read_matrix_from_file(matrix1_file);
        matrix matr2 = read_matrix_from_file(matrix2_file);
        matrix result;
    
        double time = measure_time(matr1, matr2, result);//выполняем умножение и считаем время операции
        int numberOperation = matr1.getRows()* (2*matr1.getCols() -1)* matr2.getCols(); //считаем количество операций умножения и сложения
        
        std::cout<<"Enter path to directory save matrix result: ";
        std::string result_file;
        result_file = readPath();
        result_file+="result.txt";
        write_matrix_to_file(result, result_file, numberOperation, time);
        std::cout << "\nResult saved to result.txt\n";

        bool verif = verificationOnPython(matrix1_file, matrix2_file, result_file);
        if (verif == 0) {
            std::cout << "Verification successful!" << std::endl;
        } 
        else {
            std::cout << "Verification failed!" << std::endl;
        }                
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}