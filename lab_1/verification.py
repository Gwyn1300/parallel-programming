import numpy as np
import sys

def read_matrix(filename):
    """Чтение матрицы из файла в формате: rows cols\n data..."""
    with open(filename, 'r') as f:
        # Читаем размеры
        rows, cols = map(int, f.readline().split())
        
        # Читаем данные
        matrix = []
        for _ in range(rows):
            row = list(map(float, f.readline().split()))
            if len(row) != cols:
                raise ValueError(f"Строка содержит {len(row)} элементов, ожидалось {cols}")
            matrix.append(row)
    
    return np.array(matrix)

def write_matrix(filename, matrix):
    """Запись матрицы в файл"""
    with open(filename, 'w') as f:
        rows, cols = matrix.shape
        f.write(f"{rows} {cols}\n")
        for i in range(rows):
            for j in range(cols):
                f.write(f"{matrix[i][j]:.6f} ")
            f.write("\n")

def verify_multiplication(file1, file2, result_file):
    """Проверка умножения матриц"""
    try:
        A = read_matrix(file1)
        B = read_matrix(file2)
        C_cpp = read_matrix(result_file)
                
        if A.shape[1] != B.shape[0]:
            print(f"Ошибка: Нельзя умножить {A.shape} на {B.shape}")
            return False
    
        C_python = np.dot(A, B)
        
        if np.allclose(C_cpp, C_python, rtol=1e-5, atol=1e-5):
            print("\n РЕЗУЛЬТАТ ВЕРЕН")
            return True
        else:
            print("\n РЕЗУЛЬТАТ НЕВЕРЕН")           
            return False
            
    except Exception as e:
        print(f"Ошибка: {e}")
        return False


if __name__ == "__main__":
    if len(sys.argv) == 4:
        file1, file2, result = sys.argv[1], sys.argv[2], sys.argv[3]
        success = verify_multiplication(file1, file2, result)
        sys.exit(0 if success else 1)
    
        C = np.dot(A, B)
        write_matrix("expected_result.txt", C)
        print(f"Ожидаемый результат сохранен в expected_result.txt")
    
    else:
        print("Использование:")
        print("  Верификация: python verify.py matrix1.txt matrix2.txt result.txt")