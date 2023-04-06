#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include "omploop.hpp"
#include <chrono>

#define DEBUG 0
#define PARALLEL 1

#ifdef __cplusplus
extern "C" {
#endif

  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (const int* arr, size_t n);

#ifdef __cplusplus
}
#endif

void swap(int* arr, int i, int j) {
  int temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
}


int main (int argc, char* argv[]) {
  if (argc < 3) { std::cerr<<"usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);

  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  OmpLoop parloop;
  parloop.setGranularity(std::max(10, n/100));
  for (int i=0; i<n; i++){
    for (int j=0; j<2; j++){
      #if PARALLEL
        parloop.parfor(i, 0, -1, 
                      [&](int k){
                        int a = j+(i-k)*2;
                        if(a < n-1){
                          #if DEBUG
                            std::cout << "(" << k << "," << a << ")" << ", ";
                          #endif
                          if (arr[a] > arr[a+1]){
                            swap(arr, a, a+1);
                          }
                        }
                      });
      #else 
        for(int k=i; k>=0; k--){
          int a = j + (i-k)*2;
          if(a < n-1){
            #if DEBUG
              std::cout << "(" << k << "," << a << ")" << ", ";
            #endif
            if (arr[a] > arr[a+1]){
              swap(arr, a, a+1);
            }
          }
        }
        #if DEBUG
          std::cout << "\n";
        #endif
      #endif
    }
  }
  for(int i=2; i<n-1; i++){
    for(int k=n-1; k>=0; k--){
        int a = i + ((n-1)-k)*2;
        if(a < n-1){
          #if DEBUG
            std::cout << "(" << k << "," << a << ")" << ", ";
          #endif
          if (arr[a] > arr[a+1]){
            swap(arr, a, a+1);
          }
        }
    }
  }


  
  // for(int i=0; i<n; i++){
  //   for(int j=0; j<n-1; j++){
  //     #if DEBUG
  //       std::cout << "(" << i << "," << j << ")" << ", ";
  //     #endif
  //     if(arr[j] > arr[j+1]){
  //       swap(arr, j, j+1);
  //     }
  //   }
  //   #if DEBUG
  //     std::cout << "\n";
  //   #endif
  // }
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elpased_seconds = end-start;
  std::cerr<<elpased_seconds.count()<<std::endl;

  
  checkMergeSortResult (arr, n);
  
  delete[] arr;

  return 0;
}
