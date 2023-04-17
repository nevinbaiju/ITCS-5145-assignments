#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif

float compute (float (*func)(float, int), float a, float b, int n, int intensity, int rank, int partition_size){
  int start = partition_size*rank;
  int end = std::min(n, partition_size*(rank+1));
  #if DEBUG
    std::cout << "Rank " << rank << " "<< start << ":" << end << " Partition " << partition_size << "\n";
  #endif
  float b_minus_a_over_n = (b - a)/n;
  float ans = 0;
  for (int i = start; i<end; i++){
    ans = ans + func(a + (i + 0.5)*b_minus_a_over_n, intensity);
  }
  ans = ans;

  return ans;

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

  auto start = chrono::steady_clock::now();

  switch(function_id) {
    case 1:{
      ans[0] = compute(&f1, a, b, n, intensity, rank, partition_size);
      MPI_Reduce(&(ans[0]), &(final_ans[0]), 1, MPI_INT,
                MPI_SUM, 0, MPI_COMM_WORLD);
      if(rank == 0){
        cout << final_ans[0]*((b - a)/n) << "\n";
      }
      break;
    }

    case 2:{
      float ans = compute(&f2, a, b, n, intensity, rank, partition_size);
      cout << ans << "\n";
      break;
    }

    case 3:{
      float ans = compute(&f3, a, b, n, intensity, rank, partition_size);
      cout << ans << "\n";
      break;
    }

    case 4:{
      float ans = compute(&f4, a, b, n, intensity, rank, partition_size);
      cout << ans << "\n";
      break;
    }
    default:
    cout << "Wrong function ID";
  }

  auto end = chrono::steady_clock::now();
  double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e9;

  MPI_Finalize();

  if(rank==0){
    cerr << time_taken;
  }
  return 0;



  return 0;
}
