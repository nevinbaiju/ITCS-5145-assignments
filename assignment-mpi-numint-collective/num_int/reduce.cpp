#include <mpi.h>
#include <iostream>
#include <vector>

void printvector(const std::string & msg, const std::vector<int>& v) {
    std::cout<<msg;
    for (auto& a : v)
    std::cout<<a<<" ";
    std::cout<<"\n";
}

int main (int argc, char*argv[]) {
    
    MPI_Init (&argc, &argv);
    
    int size;
    int rank;
    
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    std::cout<<"I am "<<rank<<" out of "<<size<<"\n";
    
    int vecsize = 12;
    
    std::vector<int> vec(vecsize);
    std::vector<int> vec2(vecsize);
    
    for (int i=0; i<vecsize; ++i)
        vec[i] = rank*100+i;
    
    printvector("before: ", vec);
    
    MPI_Reduce(&(vec[0]), &(vec2[0]), vecsize, MPI_INT,
                MPI_SUM, 0, MPI_COMM_WORLD);
    
    printvector("after: ", vec2);
    
    MPI_Finalize();
}
