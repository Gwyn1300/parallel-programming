#include <vector>

class matrix{ 
    size_t _rows = 0;
    size_t _cols = 0;
    std::vector<std::vector<double>> _data;
    
    public:
    matrix();  

    matrix(size_t rows, size_t cols);

    matrix(size_t rows, size_t cols, std::vector<std::vector<double>> data);
    
    size_t getRows() const;
    
    size_t getCols() const;

    void setElement(size_t i, size_t j, double value);
    
    double getElement(size_t i, size_t j) const;
    
    void fill(double value);
};