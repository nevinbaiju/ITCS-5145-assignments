#include <mpi.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char*argv[]) {
    MPI_Init (&argc, &argv);
    
    int size;
    int rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    sleep(rank);
    
    std::cout<<"I am "<<rank<<" out of "<<size<<"\n";
    std::cout<<"I am unix process :"<<getpid()<<"\n";
    
    MPI_Finalize();
}
