import csv
import os
import sys

def saveData(size: int, numberOperation: int, time: float, num_processes: int, csv_path: str) -> bool:
    """
    Сохранить данные в CSV-файл. Если файл существует - дозаписывает,
    если нет - создает новый с заголовком.
    """
    try:
        file_exists = os.path.isfile(csv_path)
        
        with open(csv_path, mode="a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            
            if not file_exists:
                writer.writerow(["Number processes", "Size", "Number Operation", "Time (seconds)"])
            
            writer.writerow([num_processes, size, numberOperation, time])
            
        action = "дозаписаны" if file_exists else "создан"
        print(f"Данные успешно {action} в файл: {csv_path}")
        return True
        
    except IOError as e:
        print(f"Ошибка при сохранении CSV: {e}")
        return False
    except Exception as e:
        print(f"Неожиданная ошибка: {e}")
        return False

if __name__ == "__main__":
    if len(sys.argv) == 6:
        size = int(sys.argv[1])
        numberOperation = int(sys.argv[2])
        time = float(sys.argv[3])
        num_processes = int(sys.argv[4])
        csv_path = sys.argv[5]
        success = saveData(size, numberOperation, time, num_processes, csv_path)
        sys.exit(0 if success else 1)
    else:
        print("Использование: python dataProcessing.py <size> <numberOperation> <time> <num_processes> <csv_path>")
        print("Пример: python dataProcessing.py 200 8000000 0.123 4 report/data_mpi.csv")
        sys.exit(1)