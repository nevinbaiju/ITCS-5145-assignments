#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
#include <vector>
#include <chrono>

#ifdef __cplusplus
extern "C" {
#endif

#define GRANULARITY 100
#define DEBUG 0

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

void compute (float (*func)(float, int), float a, float b, int n, int intensity, int rank, int size){
  std::vector<int> start_end(2);
  std::vector<int> status(1);
  std::vector<float> ans(1);
  std::vector<float> curr_ans(1);

  MPI_Status s;

  if(rank ==0){
    ans[0] = 0;
    int current_communicator = 1;
    for(int i=0; i<n; i+=GRANULARITY){
      #if DEBUG
        std::cout << "i= " << i << " "<< i+GRANULARITY << "\n";
      #endif
      if (current_communicator == size){
        for(int node=1; node<size; node++){
          MPI_Recv(&(curr_ans[0]), 2, MPI_INT, node, 69, MPI_COMM_WORLD, &s);
          #if DEBUG
            std::cout << "Receiving from Rank " << node << " "<< curr_ans[0] << "\n";
          #endif
          ans[0] = ans[0] + curr_ans[0];
        }
        current_communicator=1;
      }
      
      start_end[0] = i;
      start_end[1] = std::min(n, i+GRANULARITY);
      status[0] = 1;
      #if DEBUG
        std::cout << "Sending to Rank " << current_communicator << " "<< start_end[0] << ":" << start_end[1] << "\n";
      #endif
      MPI_Send(&(status[0]), 2, MPI_INT, current_communicator, 100, MPI_COMM_WORLD);
      MPI_Send(&(start_end[0]), 2, MPI_INT, current_communicator, 123, MPI_COMM_WORLD);
      current_communicator++;
      
    }
    #if DEBUG
          std::cout << "Iterations complete" << "\n";
    #endif
    for(int node=1; node<current_communicator; node++){
      #if DEBUG
        std::cout << "Receiving from Rank " << node << " "<< curr_ans[0] << "\n";
      #endif
      MPI_Recv(&(curr_ans[0]), 2, MPI_INT, node, 69, MPI_COMM_WORLD, &s);
      ans[0] = ans[0] + curr_ans[0];
    }
    
    for(int node=1; node<size; node++){
      status[0] = 0;
      #if DEBUG
        std::cout << "Sending signal to shut down " << node << "\n";
      #endif
      MPI_Send(&(status[0]), 2, MPI_INT, node, 100, MPI_COMM_WORLD);
    }

    #if DEBUG
      std::cout << "Shutting down " << "Root" << "\n";
    #endif
    std::cout << ans[0]*((b - a)/n) << std::endl;
  }
  else{
    MPI_Recv(&(status[0]), 2, MPI_INT, 0, 100, MPI_COMM_WORLD, &s);
    while(status[0]){
      MPI_Recv(&(start_end[0]), 2, MPI_INT, 0, 123, MPI_COMM_WORLD, &s);
      ans[0] = 0;
      for (int i = start_end[0]; i<start_end[1]; i++){
        ans[0] = ans[0] + func(a + (i + 0.5)*((b - a)/n), intensity);
      }
      MPI_Send(&(ans[0]), 2, MPI_INT, 0, 69, MPI_COMM_WORLD);
      #if DEBUG
          std::cout << "Rank " << rank << " "<< start_end[0] << ":" << start_end[1] << " Partition "<< n<< "\n";
      #endif
      MPI_Recv(&(status[0]), 2, MPI_INT, 0, 100, MPI_COMM_WORLD, &s);
    }
    #if DEBUG
      std::cout << "Shutting down " << rank << "\n";
    #endif
  }

}

int main (int argc, char* argv[]) {

  MPI_Init (&argc, &argv);
  
  int size;
  int rank;
  
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (argc < 6) {
    std::cerr<<"usage: mpirun -np <processes> "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }



  int function_id = atoi(argv[1]);
  float a = atof(argv[2]);
  float b = atof(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);

  int partition_size = ceil((double)n/size);
  std::vector<int> ans(1);
  std::vector<int> final_ans(1);

  auto start = std::chrono::steady_clock::now();

  switch(function_id) {
    case 1:{
      compute(&f1, a, b, n, intensity, rank, size);
      break;
    }

    // case 2:{
    //   float ans = compute(&f2, a, b, n, intensity, rank, partition_size);
    //   std::cout << ans << "sdfsadfsd\n";
    //   break;
    // }

    // case 3:{
    //   float ans = compute(&f3, a, b, n, intensity, rank, partition_size);
    //   std::cout << ans << "\n";
    //   break;
    // }

    // case 4:{
    //   float ans = compute(&f4, a, b, n, intensity, rank, partition_size);
    //   std::cout << ans << "\n";
    //   break;
    // }
    default:
    std::cout << "Wrong function ID";
  }

  auto end = std::chrono::steady_clock::now();
  double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e9;

  MPI_Finalize();

  if(rank==0){
    std::cerr << time_taken;
  }
  return 0;



  return 0;
}
