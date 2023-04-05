#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <chrono>
#include <unistd.h>

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
#define DEBUG_LOOP false

int main (int argc, char* argv[]) {
  if (argc < 3) {
    std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  
  int n = atoi(argv[1]);
  int num_threads = atoi(argv[2]);

  #if DEBUG_LOOP
    int arr[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
  #else
    int * arr = new int [n];
    generatePrefixSumData (arr, n);
  #endif


  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  
  int* prefix = new int [n+1];

  prefix[0] = 0;

  #if DEBUG
    for (int i=0; i<n; ++i) {
      std::cout << arr[i] << ", ";
    }
    std::cout << std::endl;
  #endif

  // for (int i=0; i<n; ++i) {
  //   prefix[i+1] = prefix[i] + arr[i];
  // }

  OmpLoop parloop;
  parloop.setGranularity(n/10);


  int partition = n/10;//std::max(10, (int)n/100);
  parloop.setGranularity(5);
  parloop.setNbThread(num_threads);
  parloop.parfor(0, n, partition, 
                [&](int i){
                  prefix[i+1] = arr[i];
                  for(int j=i+2; j<=i+partition; j++){
                    // std::cout << j << std::endl;
                    prefix[j] = prefix[j-1] + arr[j-1];
                  }
                }
);
#if DEBUG
    for (int i=1; i<=n; ++i) {
      std::cout << prefix[i] << ", ";
    }
    std::cout << std::endl;
  #endif
int granularity = partition/100;
parloop.setGranularity(100000);
for(int i=partition; i<n; i+=partition){
  int prev_sum = prefix[i];
  #if DEBUG
    std::cout << prev_sum << " Prev sum" << std::endl;
    std::cout << i << " " << i+granularity << std::endl;
  #endif
  parloop.parfor(i+1, i+partition+1, 1,
                 [&](int j){
                      prefix[j] = prefix[j]+prev_sum;
                 });
}
  
  

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

  #if !DEBUG_LOOP
    delete[] arr;
  #endif
  delete[] prefix;

  return 0;
}
