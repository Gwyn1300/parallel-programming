import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

def plot_threads_performance(csv_path: str, output_dir: str = None):
    """
    Построение графиков зависимости времени от количества потоков для каждого размера матрицы
    
    Параметры:
    csv_path: путь к CSV файлу с данными
    output_dir: директория для сохранения графиков (если None, то рядом с CSV)
    """
    try:
        df = pd.read_csv(csv_path)
        
        print(f"Диапазон потоков: {df['Number threads'].min()} - {df['Number threads'].max()}")
        
        if output_dir is None:
            output_dir = os.path.dirname(csv_path)
        os.makedirs(output_dir, exist_ok=True)
        
        plt.style.use('seaborn-v0_8-darkgrid')
        
        sizes = sorted(df['Size'].unique())
        
        for size in sizes:
            df_size = df[df['Size'] == size].sort_values('Number threads')
            
            if len(df_size) == 0:
                continue
            
            fig, ax = plt.subplots(figsize=(10, 6))
            
            ax.plot(df_size['Number threads'], df_size['Time (seconds)'], 
                   'bo-', linewidth=2, markersize=8, markerfacecolor='white', markeredgewidth=2)
            
            for _, row in df_size.iterrows():
                ax.annotate(f'{row["Time (seconds)"]:.4f} с', 
                           (row['Number threads'], row['Time (seconds)']), 
                           textcoords="offset points", xytext=(0, 10), 
                           ha='center', fontsize=8,
                           bbox=dict(boxstyle='round,pad=0.3', facecolor='yellow', alpha=0.7))
            
            ax.set_xlabel('Количество потоков', fontsize=12)
            ax.set_ylabel('Время выполнения (секунды)', fontsize=12)
            ax.set_title(f'Зависимость времени от количества потоков\nРазмер матрицы: {size}x{size}', fontsize=14)
            ax.grid(True, alpha=0.3)
            ax.set_xticks(df_size['Number threads'].unique())
            
            output_path = os.path.join(output_dir, f'performance_threads_{size}x{size}.png')
            plt.savefig(output_path, dpi=150, bbox_inches='tight')
            print(f"График сохранен: {output_path}")
            
            plt.close() 
    
        fig, ax = plt.subplots(figsize=(12, 8))
        
        colors = plt.cm.viridis(np.linspace(0, 1, len(sizes)))
        
        for idx, size in enumerate(sizes):
            df_size = df[df['Size'] == size].sort_values('Number threads')
            ax.plot(df_size['Number threads'], df_size['Time (seconds)'], 
                   'o-', linewidth=2, markersize=6, color=colors[idx],
                   label=f'{size}x{size}')
        
        ax.set_xlabel('Количество потоков', fontsize=12)
        ax.set_ylabel('Время выполнения (секунды)', fontsize=12)
        ax.set_title('Зависимость времени от количества потоков для разных размеров матриц', fontsize=14)
        ax.legend(loc='best', fontsize=10)
        ax.grid(True, alpha=0.3)
        
        output_path = os.path.join(output_dir, 'performance_threads_all_sizes.png')
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        print(f"Общий график сохранен: {output_path}")
        
        plt.close()
        
        fig, ax = plt.subplots(figsize=(12, 8))
        
        for idx, size in enumerate(sizes):
            df_size = df[df['Size'] == size].sort_values('Number threads')
            
            base_time = df_size[df_size['Number threads'] == 1]['Time (seconds)'].values[0]
            
            speedup = base_time / df_size['Time (seconds)']
            
            ax.plot(df_size['Number threads'], speedup, 
                   'o-', linewidth=2, markersize=6, color=colors[idx],
                   label=f'{size}x{size}')
        
        max_threads = df['Number threads'].max()
        ax.plot([1, max_threads], [1, max_threads], 'k--', linewidth=1.5, 
               label='Идеальное ускорение', alpha=0.7)
        
        ax.set_xlabel('Количество потоков', fontsize=12)
        ax.set_ylabel('Ускорение', fontsize=12)
        ax.set_title('Ускорение от количества потоков для разных размеров матриц', fontsize=14)
        ax.legend(loc='best', fontsize=10)
        ax.grid(True, alpha=0.3)
        ax.set_xlim(1, max_threads)
        ax.set_ylim(0, max_threads + 1)
        
        output_path = os.path.join(output_dir, 'speedup_all_sizes.png')
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        print(f"График ускорения сохранен: {output_path}")
        
        plt.close()
         
        
        print("\n" + "="*70)
        print("СТАТИСТИКА ПРОИЗВОДИТЕЛЬНОСТИ")
        print("="*70)
        
        for size in sizes:
            df_size = df[df['Size'] == size].sort_values('Number threads')
            best_idx = df_size['Time (seconds)'].idxmin()
            best_threads = df_size.loc[best_idx, 'Number threads']
            best_time = df_size.loc[best_idx, 'Time (seconds)']
            base_time = df_size[df_size['Number threads'] == 1]['Time (seconds)'].values[0]
            
            print(f"\nРазмер {size}x{size}:")
            print(f"  Лучшее время: {best_time:.4f} сек на {best_threads} потоках")
            print(f"  Ускорение: {base_time/best_time:.2f}x")
        
        print("\n" + "="*70)
        
        return True
        
    except Exception as e:
        print(f"Ошибка при построении графиков: {e}")
        import traceback
        traceback.print_exc()
        return False

def plot_single_size_analysis(csv_path: str, size: int, output_dir: str = None):
    """
    Построение детального анализа для одного размера матрицы
    """
    try:
        df = pd.read_csv(csv_path)
        df_size = df[df['Size'] == size].sort_values('Number threads')
        
        if len(df_size) == 0:
            print(f"Нет данных для размера {size}")
            return False
        
        if output_dir is None:
            output_dir = os.path.dirname(csv_path)
        os.makedirs(output_dir, exist_ok=True)
        
        fig, axes = plt.subplots(1, 2, figsize=(15, 5))
        fig.suptitle(f'Анализ производительности для матрицы {size}x{size}', fontsize=14)
        
        threads = df_size['Number threads'].values
        times = df_size['Time (seconds)'].values
        base_time = times[0]
        
        ax1 = axes[0]
        ax1.plot(threads, times, 'bo-', linewidth=2, markersize=8)
        ax1.set_xlabel('Количество потоков')
        ax1.set_ylabel('Время (секунды)')
        ax1.set_title('Время выполнения')
        ax1.grid(True, alpha=0.3)
        ax1.set_xticks(threads)
        
        ax2 = axes[1]
        speedup = base_time / times
        ax2.plot(threads, speedup, 'go-', linewidth=2, markersize=8)
        ax2.plot(threads, threads, 'r--', linewidth=1.5, alpha=0.7, label='Идеальное')
        ax2.set_xlabel('Количество потоков')
        ax2.set_ylabel('Ускорение')
        ax2.set_title('Ускорение')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        ax2.set_xticks(threads)
        
        
        plt.tight_layout()
        
        output_path = os.path.join(output_dir, f'analysis_{size}x{size}.png')
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        print(f"Анализ для размера {size} сохранен: {output_path}")
        
        plt.close()
        return True
        
    except Exception as e:
        print(f"Ошибка: {e}")
        return False

if __name__ == "__main__":
    if len(sys.argv) > 1:
        csv_path = sys.argv[1]
        output_dir = sys.argv[2] if len(sys.argv) > 2 else None
        
        print("="*60)
        print("ПОСТРОЕНИЕ ГРАФИКОВ ПРОИЗВОДИТЕЛЬНОСТИ")
        print("="*60)
        
        plot_threads_performance(csv_path, output_dir)
        
        df = pd.read_csv(csv_path)
        for size in sorted(df['Size'].unique()):
            plot_single_size_analysis(csv_path, size, output_dir)
        
        print("\nВсе графики успешно созданы!")
        
    else:
        print("Использование: python dataVisualization.py <путь_к_csv> [директория_вывода]")
        print("Пример: python dataVisualization.py report/data.csv report/")
        