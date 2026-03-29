import csv
import os
import sys

def saveData(size: int, numberOperation: int, time: float, num_threads:int, csv_path: str) -> None:
    """
    Сохранить данные в CSV-файл. Если файл существует - дозаписывает,
    если нет - создает новый с заголовком.
    """
    try:
        file_exists = os.path.isfile(csv_path)
        
        with open(csv_path, mode="a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            
            if not file_exists:
                writer.writerow(["Number threads","Size", "Number Operation", "Time (seconds)"])
            
            writer.writerow([num_threads, size, numberOperation, time])
            
        action = "дозаписаны" if file_exists else "создан"
        print(f"Данные успешно {action} в файл: {csv_path}")
        
    except IOError as e:
        print(f"Ошибка при сохранении CSV: {e}")
        raise
    except Exception as e:
        print(f"Неожиданная ошибка: {e}")
        raise

if __name__ == "__main__":
    if len(sys.argv) == 6:
        size, numberOperation, time, num_threads, csv_path = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5]
        success = saveData(size, numberOperation, time, num_threads, csv_path)
        sys.exit(0 if success else 1)
    
    else:
        print("Использование:")