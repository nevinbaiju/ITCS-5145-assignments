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
  void generateMergeSortData (int* arr, size_t n);
  void checkMergeSortResult (int* arr, size_t n);
#ifdef __cplusplus
}
#endif

#define DEBUG 0

// geeksforgeeks - https://www.geeksforgeeks.org/iterative-merge-sort/
void merge(int * arr, int l, int m, int r)
{
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;
 
    std::vector<int> L (n1);
    std::vector<int> R(n2);

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];
 
    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2)
    {
        // std::cout << n1 << " " << i << " " << n2 << " " << j << " " << std::endl;
        if (L[i] <= R[j])
        {
            arr[k] = L[i];
            i++;
        }
        else
        {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
 
    while (i < n1)
    {
        arr[k] = L[i];
        i++;
        k++;
    }
 
    while (j < n2)
    {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void print_arr(int * arr, int start, int end){
  #if DEBUG
    std::cout << start << ":" << end << " => ";
    // for(int i=start; i<=end; i++){
    //   std::cout << arr[i] << ", ";
    // }
    std::cout << std::endl;
  #endif
}

void mergesort(int * arr, int n, int num_threads)
{
   int curr_size;
   int left_start;
   
   OmpLoop parloop;
   parloop.setNbThread(num_threads);
   parloop.setGranularity(std::max(10, n/100));
   int merge_counter = 0;
   bool break_condition = false;
   for (curr_size=1; curr_size<=n-1; curr_size = 2*curr_size)
   {
      parloop.parfor(0, n-1, 2*curr_size, 
                    [&](int left_start){
                        int mid = std::min(left_start + curr_size - 1, n-1);
                        int right_end = std::min(left_start + 2*curr_size - 1, n-1);
                        print_arr(arr, left_start, mid);
                        print_arr(arr, mid+1, right_end);
                        // Merge Subarrays arr[left_start...mid] & arr[mid+1...right_end]
                        merge(arr, left_start, mid, right_end);

                        print_arr(arr, left_start, right_end);
                    });
      //  for (left_start=0; left_start<n-1; left_start += 2*curr_size)
      //  {
      //     if (left_start >= n){
      //       break_condition = true;
      //       break;
      //     }
      //      int mid = std::min(left_start + curr_size - 1, n-1);
 
      //      int right_end = std::min(left_start + 2*curr_size - 1, n-1);

      //      print_arr(arr, left_start, mid);
      //      print_arr(arr, mid+1, right_end);
      //      merge(arr, left_start, mid, right_end);
      //      merge_counter++;
      //      print_arr(arr, left_start, right_end);
      //  }
       if (break_condition){
        break;
       }
   }
}

int main (int argc, char* argv[]) {
  
  if (argc < 3) { std::cerr<<"Usage: "<<argv[0]<<" <n> <nbthreads>"<<std::endl;
    return -1;
  }

  int n = atoi(argv[1]);
  int num_threads = atoi(argv[2]);
  
  // get arr data
  int * arr = new int [n];
  generateMergeSortData (arr, n);

  // begin timing
  std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
  print_arr(arr, 0, n-1);
  mergesort(arr, n, num_threads);
  print_arr(arr, 0, n-1);
  
  // end timing
  std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();
  std::chrono::duration<double> elpased_seconds = end-start;

  // display time to cerr
  std::cerr<<elpased_seconds.count()<<std::endl;
  checkMergeSortResult (arr, n);
  
  delete[] arr;

  return 0;
}
