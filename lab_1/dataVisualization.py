import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

def plot_graphs(csv_path: str, output_dir: str = None):
    """
    Построение графиков на основе CSV файла с данными экспериментов
    
    Параметры:
    csv_path: путь к CSV файлу с данными
    output_dir: директория для сохранения графиков (если None, то рядом с CSV)
    """
    try:
        # Чтение данных из CSV
        df = pd.read_csv(csv_path)
        
        # Создаем директорию для графиков
        if output_dir is None:
            output_dir = os.path.dirname(csv_path)
        os.makedirs(output_dir, exist_ok=True)
        
        # Настройка стиля
        plt.style.use('seaborn-v0_8-darkgrid')
        
        # Создание фигуры с двумя подграфиками
        fig, axes = plt.subplots(1, 2, figsize=(14, 6))
        fig.suptitle('Анализ производительности умножения матриц', fontsize=16)
        
        # 1. График: Время от размера матрицы
        ax1 = axes[0]
        ax1.plot(df['Size'], df['Time (seconds)'], 'bo-', linewidth=2, markersize=8, 
                markerfacecolor='white', markeredgewidth=2)
        
        ax1.set_xlabel('Размер матрицы (n)', fontsize=12)
        ax1.set_ylabel('Время выполнения (секунды)', fontsize=12)
        ax1.set_title('Зависимость времени от размера матрицы', fontsize=14)
        ax1.grid(True, alpha=0.3)
        
        # Добавляем значения на график
        for i, (size, time) in enumerate(zip(df['Size'], df['Time (seconds)'])):
            ax1.annotate(f'{time:.3f} с', (size, time), textcoords="offset points", 
                        xytext=(0, 10), ha='center', fontsize=9,
                        bbox=dict(boxstyle='round,pad=0.3', facecolor='yellow', alpha=0.7))
        
        # 2. График: Время от количества операций
        ax2 = axes[1]
        operations = df['Number Operation'] / 1e9  # в миллиардах для читаемости
        
        ax2.plot(operations, df['Time (seconds)'], 'go-', linewidth=2, markersize=8,
                markerfacecolor='white', markeredgewidth=2)
        
        ax2.set_xlabel('Количество операций (миллиарды)', fontsize=12)
        ax2.set_ylabel('Время выполнения (секунды)', fontsize=12)
        ax2.set_title('Зависимость времени от количества операций', fontsize=14)
        ax2.grid(True, alpha=0.3)
        
        # Добавляем значения на график
        for i, (ops, time) in enumerate(zip(operations, df['Time (seconds)'])):
            ax2.annotate(f'{time:.3f} с', (ops, time), textcoords="offset points", 
                        xytext=(0, 10), ha='center', fontsize=9,
                        bbox=dict(boxstyle='round,pad=0.3', facecolor='lightgreen', alpha=0.7))
        
        plt.tight_layout()
        
        # Сохраняем график
        output_path = os.path.join(output_dir, 'matrix_performance.png')
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        print(f"\nГрафики сохранены в: {output_path}")
        
        plt.show()
        
        return True
        
    except Exception as e:
        print(f"Ошибка при построении графиков: {e}")
        import traceback
        traceback.print_exc()
        return False


def plot_combined_graphs(csv_path: str, output_dir: str = None):
    """
    Комбинированные графики на одной фигуре
    """
    try:
        df = pd.read_csv(csv_path)
        
        if output_dir is None:
            output_dir = os.path.dirname(csv_path)
        os.makedirs(output_dir, exist_ok=True)
        
        plt.style.use('seaborn-v0_8-darkgrid')
        
        fig, axes = plt.subplots(2, 2, figsize=(14, 10))
        fig.suptitle('Полный анализ производительности умножения матриц', fontsize=16)
        
        operations_billions = df['Number Operation'] / 1e9
        
        # 1. Время от размера (линейный)
        ax1 = axes[0, 0]
        ax1.plot(df['Size'], df['Time (seconds)'], 'bo-', linewidth=2, markersize=8)
        ax1.set_xlabel('Размер матрицы')
        ax1.set_ylabel('Время (с)')
        ax1.set_title('Время от размера (линейный)')
        ax1.grid(True, alpha=0.3)
        
        # 2. Время от операций (линейный)
        ax2 = axes[0, 1]
        ax2.plot(operations_billions, df['Time (seconds)'], 'go-', linewidth=2, markersize=8)
        ax2.set_xlabel('Операции (млрд)')
        ax2.set_ylabel('Время (с)')
        ax2.set_title('Время от операций (линейный)')
        ax2.grid(True, alpha=0.3)
        
        # 3. Время от размера (логарифмический)
        ax3 = axes[1, 0]
        ax3.loglog(df['Size'], df['Time (seconds)'], 'bo-', linewidth=2, markersize=8)
        ax3.set_xlabel('Размер матрицы')
        ax3.set_ylabel('Время (с)')
        ax3.set_title('Время от размера (log-log)')
        ax3.grid(True, alpha=0.3, which='both')
        
        # 4. Время от операций (логарифмический)
        ax4 = axes[1, 1]
        ax4.loglog(df['Number Operation'], df['Time (seconds)'], 'go-', linewidth=2, markersize=8)
        ax4.set_xlabel('Количество операций')
        ax4.set_ylabel('Время (с)')
        ax4.set_title('Время от операций (log-log)')
        ax4.grid(True, alpha=0.3, which='both')
        
        plt.tight_layout()
        
        output_path = os.path.join(output_dir, 'matrix_performance_combined.png')
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        print(f"Комбинированные графики сохранены в: {output_path}")
        
        plt.show()
        
        return True
        
    except Exception as e:
        print(f"Ошибка при построении комбинированных графиков: {e}")
        return False

if __name__ == "__main__":
    if len(sys.argv) > 1:
        csv_path = sys.argv[1]
        
        # Опционально: второй аргумент - директория для сохранения
        output_dir = sys.argv[2] if len(sys.argv) > 2 else None
            
        # Обычные графики
        plot_graphs(csv_path, output_dir)
        
        
        print("\nВсе графики успешно созданы!")
        
    else:
        print("Использование: python data_visualization.py <путь_к_csv> [директория_вывода]")
        print("Пример: python data_visualization.py report/data.csv report/")
        print("\nПримеры данных в CSV:")
        print("Size,Number Operation,Time (seconds)")
        print("200,15960000,0.113297")
        print("800,1023360000,6.478444")
        print("1200,3454560000,23.258380")