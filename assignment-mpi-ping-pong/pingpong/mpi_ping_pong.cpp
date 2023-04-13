#include <mpi.h>
#include <iostream>
#include <vector>

void printvector(const std::string & msg, const std::vector<int>& v) {
  std::cout<<msg;
  for (auto& a : v)
    std::cout<<a<<" ";
  std::cout<<"\n";
}

int main (int argc, char* argv[]) {

  MPI_Init (&argc, &argv);
  
  int size;
  int rank;
  
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc < 2) {
    
    std::cerr<<"usage: mpirun "<<argv[0]<<" <value>"<<std::endl;
    
    return -1;
  }

  int vecsize = 1;
  std::vector<int> vec(vecsize);
  std::vector<int> vec2(vecsize);


  if (rank == 0) {
    vec[0] = atoi(argv[1]);
    MPI_Send(&(vec[0]), vecsize, MPI_INT, 1, 123, MPI_COMM_WORLD);
    MPI_Status s;
    MPI_Recv(&(vec[0]), vecsize, MPI_INT, 1, 123, MPI_COMM_WORLD, &s);
    std::cout << vec[0] << std::endl;
  }
  else if (rank == 1) {
    MPI_Status s;
    MPI_Recv(&(vec2[0]), vecsize, MPI_INT, 0, 123, MPI_COMM_WORLD, &s);
    vec2[0] = vec2[0] + 2;
    MPI_Send(&(vec2[0]), vecsize, MPI_INT, 0, 123, MPI_COMM_WORLD);
  }
  
  MPI_Finalize();  


  return 0;
}
