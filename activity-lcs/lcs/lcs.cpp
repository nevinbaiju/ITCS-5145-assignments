#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include "omploop.hpp"

#ifdef __cplusplus
extern "C" {
#endif

  void generateLCS(char* X, int m, char* Y, int n);
  void checkLCS(char* X, int m, char* Y, int n, int result);

#ifdef __cplusplus
}
#endif

int parallelLCS(char* X, int m, char* Y, int n, int num_threads) {

  int** C = new int*[m+1];

  OmpLoop parloop;
  parloop.setNbThread(num_threads);

  parloop.setGranularity(1);
  parloop.parfor(0, m+1, 1,
                [&](int i){
                  C[i] = new int[n+1];
                  C[i][0] = 0;
                }
  );

  parloop.parfor(0, n+1, 1,
                [&](int i){
                  C[0][i] = 0;
                }
  );
  
  parloop.setGranularity(10000);
  for (int k = 0; k < m + n - 1; k++) {
    parloop.parfor(0, m, 1,
                [&](int i){
                int j = k - i, a, b;
                if (j >= 0 && j < n) {
                  a = i+1;
                  b = j+1;
                  if (X[a-1] == Y[b-1]) {
                    C[a][b] = C[a-1][b-1] + 1; 
                  } else {
                    C[a][b] = std::max(C[a-1][b], C[a][b-1]);
                  }
                }
                });


    // for (int i = 0; i < m; i++) {
    //   int j = k - i;
    //   if (j >= 0 && j < n) {
    //     a = i+1;
    //     b = j+1;
    //     if (X[a-1] == Y[b-1]) {
    //       C[a][b] = C[a-1][b-1] + 1; 
    //     } else {
    //       C[a][b] = std::max(C[a-1][b], C[a][b-1]);
    //     }
    //   }
    // }
    // std::cout << std::endl;
  }


  // for (int a=1; a<=m; ++a) {
  //   for (int b=1; b<=n; ++b) {
  //     // std::cout << a << " " << b << " : ";
  //     if (X[a-1] == Y[b-1]) {
  //       C[a][b] = C[a-1][b-1] + 1; 
  //     } else {
  //       C[a][b] = std::max(C[a-1][b], C[a][b-1]);
  //     }
  //   }
  //   // std::cout << std::endl;
  // }
  int result = C[m][n];

  for (int i=0; i<=m; ++i) { 
    delete[] C[i];
  }
  delete[] C;
  
  return result;
}

int main (int argc, char* argv[]) {

  if (argc < 4) { std::cerr<<"usage: "<<argv[0]<<" <m> <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int m = atoi(argv[1]);
  int n = atoi(argv[2]);
  int num_threads = atoi(argv[3]);

  // get string data 
  char *X = new char[m];
  char *Y = new char[n];
  generateLCS(X, m, Y, n);

  
  //insert LCS code here.
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  int result = -1; // length of common subsequence

  result = parallelLCS(X, m, Y, n, num_threads);
  checkLCS(X, m, Y, n, result);

  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elpased_seconds = end-start;
  std::cerr<<elpased_seconds.count()<<std::endl;
  delete[] X;
  delete[] Y;

  return 0;
}
