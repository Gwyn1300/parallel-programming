import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import sys
import os

def plot_mpi_performance(csv_path: str, output_dir: str = None):
    """
    Построение графиков зависимости времени от количества процессов MPI для каждого размера матрицы
    """
    try:
        df = pd.read_csv(csv_path)
        
        print(f"Диапазон процессов: {df['Number processes'].min()} - {df['Number processes'].max()}")
        
        if output_dir is None:
            output_dir = os.path.dirname(csv_path)
        os.makedirs(output_dir, exist_ok=True)
        
        plt.style.use('seaborn-v0_8-darkgrid')
        
        sizes = sorted(df['Size'].unique())
        
        for size in sizes:
            df_size = df[df['Size'] == size].sort_values('Number processes')
            
            if len(df_size) == 0:
                continue
            
            fig, ax = plt.subplots(figsize=(10, 6))
            
            ax.plot(df_size['Number processes'], df_size['Time (seconds)'], 
                   'bo-', linewidth=2, markersize=8, markerfacecolor='white', markeredgewidth=2)
            
            for _, row in df_size.iterrows():
                ax.annotate(f'{row["Time (seconds)"]:.4f} с', 
                           (row['Number processes'], row['Time (seconds)']), 
                           textcoords="offset points", xytext=(0, 10), 
                           ha='center', fontsize=8,
                           bbox=dict(boxstyle='round,pad=0.3', facecolor='yellow', alpha=0.7))
            
            ax.set_xlabel('Количество процессов MPI', fontsize=12)
            ax.set_ylabel('Время выполнения (секунды)', fontsize=12)
            ax.set_title(f'Зависимость времени от количества процессов MPI\nРазмер матрицы: {size}x{size}', fontsize=14)
            ax.grid(True, alpha=0.3)
            ax.set_xticks(df_size['Number processes'].unique())
            
            output_path = os.path.join(output_dir, f'mpi_performance_{size}x{size}.png')
            plt.savefig(output_path, dpi=150, bbox_inches='tight')
            print(f"График сохранен: {output_path}")
            
            plt.close()
    
        fig, ax = plt.subplots(figsize=(12, 8))
        
        colors = plt.cm.viridis(np.linspace(0, 1, len(sizes)))
        
        for idx, size in enumerate(sizes):
            df_size = df[df['Size'] == size].sort_values('Number processes')
            ax.plot(df_size['Number processes'], df_size['Time (seconds)'], 
                   'o-', linewidth=2, markersize=6, color=colors[idx],
                   label=f'{size}x{size}')
        
        ax.set_xlabel('Количество процессов MPI', fontsize=12)
        ax.set_ylabel('Время выполнения (секунды)', fontsize=12)
        ax.set_title('Зависимость времени от количества процессов MPI для разных размеров матриц', fontsize=14)
        ax.legend(loc='best', fontsize=10)
        ax.grid(True, alpha=0.3)
        
        output_path = os.path.join(output_dir, 'mpi_performance_all_sizes.png')
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        print(f"Общий график сохранен: {output_path}")
        
        plt.close()
        
        fig, ax = plt.subplots(figsize=(12, 8))
        
        for idx, size in enumerate(sizes):
            df_size = df[df['Size'] == size].sort_values('Number processes')
            
            base_time = df_size[df_size['Number processes'] == 1]['Time (seconds)'].values[0]
            
            speedup = base_time / df_size['Time (seconds)']
            
            ax.plot(df_size['Number processes'], speedup, 
                   'o-', linewidth=2, markersize=6, color=colors[idx],
                   label=f'{size}x{size}')
        
        max_procs = df['Number processes'].max()
        ax.plot([1, max_procs], [1, max_procs], 'k--', linewidth=1.5, 
               label='Идеальное ускорение', alpha=0.7)
        
        ax.set_xlabel('Количество процессов MPI', fontsize=12)
        ax.set_ylabel('Ускорение', fontsize=12)
        ax.set_title('Ускорение от количества процессов MPI для разных размеров матриц', fontsize=14)
        ax.legend(loc='best', fontsize=10)
        ax.grid(True, alpha=0.3)
        ax.set_xlim(1, max_procs)
        ax.set_ylim(0, max_procs + 1)
        
        output_path = os.path.join(output_dir, 'mpi_speedup_all_sizes.png')
        plt.savefig(output_path, dpi=150, bbox_inches='tight')
        print(f"График ускорения сохранен: {output_path}")
        
        plt.close()
        
        print("\n" + "="*70)
        print("СТАТИСТИКА ПРОИЗВОДИТЕЛЬНОСТИ MPI")
        print("="*70)
        
        for size in sizes:
            df_size = df[df['Size'] == size].sort_values('Number processes')
            best_idx = df_size['Time (seconds)'].idxmin()
            best_procs = df_size.loc[best_idx, 'Number processes']
            best_time = df_size.loc[best_idx, 'Time (seconds)']
            base_time = df_size[df_size['Number processes'] == 1]['Time (seconds)'].values[0]
            
            print(f"\nРазмер {size}x{size}:")
            print(f"  Лучшее время: {best_time:.4f} сек на {best_procs} процессах")
            print(f"  Ускорение: {base_time/best_time:.2f}x")
        
        print("\n" + "="*70)
        
        return True
        
    except Exception as e:
        print(f"Ошибка при построении графиков: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    if len(sys.argv) > 1:
        csv_path = sys.argv[1]
        output_dir = sys.argv[2] if len(sys.argv) > 2 else None
        
        print("="*60)
        print("ПОСТРОЕНИЕ ГРАФИКОВ ПРОИЗВОДИТЕЛЬНОСТИ MPI")
        print("="*60)
        
        plot_mpi_performance(csv_path, output_dir)
        
        print("\nВсе графики успешно созданы!")
        
    else:
        print("Использование: python dataVisualization.py <путь_к_csv> [директория_вывода]")
        print("Пример: python dataVisualization.py report/data_mpi.csv report/")