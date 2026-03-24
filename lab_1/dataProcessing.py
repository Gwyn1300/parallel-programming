import csv
import os
import sys

def saveData(size: int, numberOperation: int, time: float, csv_path: str) -> None:
    """
    Сохранить данные в CSV-файл. Если файл существует - дозаписывает,
    если нет - создает новый с заголовком.
    """
    try:
        # Проверяем, существует ли файл
        file_exists = os.path.isfile(csv_path)
        
        # Открываем файл в режиме добавления (append)
        with open(csv_path, mode="a", newline="", encoding="utf-8") as f:
            writer = csv.writer(f)
            
            # Если файл только что создан (не существовал), пишем заголовок
            if not file_exists:
                writer.writerow(["Size", "Number Operation", "Time (seconds)"])
            
            # Записываем данные
            writer.writerow([size, numberOperation, time])
            
        action = "дозаписаны" if file_exists else "создан"
        print(f"Данные успешно {action} в файл: {csv_path}")
        
    except IOError as e:
        print(f"Ошибка при сохранении CSV: {e}")
        raise
    except Exception as e:
        print(f"Неожиданная ошибка: {e}")
        raise

if __name__ == "__main__":
    if len(sys.argv) == 5:
        size, numberOperation, time, csv_path = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
        success = saveData(size, numberOperation, time, csv_path)
        sys.exit(0 if success else 1)
    
    else:
        print("Использование:")