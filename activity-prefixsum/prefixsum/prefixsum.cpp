#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>

#include "omploop.hpp"

#ifdef __cplusplus
extern "C" {
#endif
  void generatePrefixSumData (int* arr, size_t n);
  void checkPrefixSumResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

#define DEBUG false  

int main (int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  
  int n = atoi(argv[1]);
  int num_threads = atoi(argv[2]);

  int * arr = new int [n];
  generatePrefixSumData (arr, n);

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  int* prefix = new int [n+1];

  prefix[0] = 0;

  #if DEBUG
    for (int i=0; i<n; ++i) {
      std::cout << arr[i] << ", ";
    }
    std::cout << std::endl;
  #endif

  for (int i=0; i<n; ++i) {
    prefix[i+1] = prefix[i] + arr[i];
  }

  OmpLoop parloop;
  int granularity = std::max(10, (int)n/100);
  parloop.setGranularity(granularity);
  parloop.setNbThread(num_threads);
  parloop.parfor(0, n, 1, 
                [&](int i){
                  prefix[i+1] = prefix[i] + arr[i];
                });
  
  #if DEBUG
    for (int i=1; i<=n; ++i) {
      std::cout << prefix[i] << ", ";
    }
    std::cout << std::endl;
  #endif


  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

  std::chrono::duration<double> elapsed_seconds = end-start;
  
  std::cerr<<elapsed_seconds.count()<<std::endl;

  
  
  checkPrefixSumResult(prefix, n);

  delete[] arr;
  delete[] prefix;

  return 0;
}
