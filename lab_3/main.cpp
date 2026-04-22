#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <mpi.h>
#include "function.h"
#include "globals.h"

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    if (rank == 0) {
        std::cout << "========================================" << std::endl;
        std::cout << "MPI Matrix Multiplication Benchmark" << std::endl;
        std::cout << "========================================" << std::endl;
        std::cout << "Initialized with " << size << " processes" << std::endl;
        std::cout << "Starting matrix multiplication experiments..." << std::endl;
        
        creatMatrixForExperement();
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    std::vector<int> process_counts;
    
    if (argc > 1) {
        int specified_count = std::atoi(argv[1]);
        if (specified_count > 0 && specified_count <= size) {
            process_counts.push_back(specified_count);
        }
        else {
            if (rank == 0) {
                std::cerr << "Invalid process count specified. Using all processes." << std::endl;
            }
            process_counts.push_back(size);
        }
    }
    else {
        if (size >= 1) process_counts.push_back(1);
        if (size >= 2) process_counts.push_back(2);
        if (size >= 4) process_counts.push_back(4);
        if (size >= 8) process_counts.push_back(8);
        if (size >= 16) process_counts.push_back(16);
        
        if (process_counts.empty()) {
            process_counts.push_back(size);
        }
    }
    
    if (rank == 0) {
        std::cout << "\nRunning experiments with processes: ";
        for (int p : process_counts) {
            std::cout << p << " ";
        }
        std::cout << std::endl << std::endl;
    }
    
    for (int num_procs : process_counts) {
        if (rank == 0) {
            std::cout << "\n=== Experiment with " << num_procs << " processes ===" << std::endl;
        }
        
        if (num_procs < size) {
            MPI_Comm small_comm;
            MPI_Comm_split(MPI_COMM_WORLD, rank < num_procs ? 0 : MPI_UNDEFINED, rank, &small_comm);
            
            if (rank < num_procs) {
                int small_rank, small_size;
                MPI_Comm_rank(small_comm, &small_rank);
                MPI_Comm_size(small_comm, &small_size);
                
                // Запуск экспериментов в подгруппе
                for (size_t i = 0; i < MATRIX.size(); i += 2) {
                    experiment(MATRIX[i], MATRIX[i + 1], num_procs);
                }
                
                MPI_Comm_free(&small_comm);
            }
        }
        else {
            for (size_t i = 0; i < MATRIX.size(); i += 2) {
                experiment(MATRIX[i], MATRIX[i + 1], num_procs);
            }
        }
        
        MPI_Barrier(MPI_COMM_WORLD);
        
        if (rank == 0) {
            std::cout << "=== Completed experiments with " << num_procs << " processes ===" << std::endl;
        }
    }
    
    if (rank == 0) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "Creating graphs from collected data..." << std::endl;
        std::cout << "========================================" << std::endl;
        
        bool graphs_created = createGraphsFromCSV(BASEPATH + "report/data_mpi.csv");
        
        if (graphs_created) {
            std::cout << "\nGraphs created successfully!" << std::endl;
        }
        else {
            std::cout << "\nWarning: Could not create graphs." << std::endl;
        }
        
        std::cout << "\nAll experiments completed!" << std::endl;
    }
    
    MPI_Finalize();
    return 0;
}