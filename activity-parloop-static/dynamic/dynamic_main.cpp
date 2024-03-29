#include <iostream>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamic_loop.cpp"

#ifdef __cplusplus
extern "C" {
#endif

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);


float compute (float (*func)(float, int), float a, float b, int n, int intensity, int num_threads, int granularity){
  DynamicLoop dl(num_threads, granularity);
  float b_minus_a_over_n = (b - a)/n;
  float ans = 0;

  dl.parfor<float>(0, n, 1,
		 [&](float& tls) -> void{
		   tls = 0;
		 },
		 [&](int i, float& tls) -> void{
		   tls += func(a + (i + 0.5)*b_minus_a_over_n, intensity);
      //  std::cout << i << " " << tls << "\n";
		  },
		 [&](float tls) -> void{
		   ans += tls;
		 }
		 );
  // std::cout << ans << std::endl;
  ans = ans*b_minus_a_over_n;

  return ans;

}

#ifdef __cplusplus
}
#endif

int main (int argc, char* argv[]) {

  if (argc < 8) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity> <nbthreads> <granularity>"<<std::endl;
    return -1;
  }
  int function_id = atoi(argv[1]);
  float a = atof(argv[2]);
  float b = atof(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);
  int num_threads = atoi(argv[6]);
  int granularity = atoi(argv[7]);

  auto start = std::chrono::steady_clock::now();
  switch(function_id) {
    case 1:{
      float ans = compute(&f1, a, b, n, intensity, num_threads, granularity);
      std::cout << ans << "\n";
      // std::cout << compute_og(&f1, a, b, n, intensity) << "\n";
      break;
    }

    case 2:{
      float ans = compute(&f2, a, b, n, intensity, num_threads, granularity);
      std::cout << ans << "\n";
      break;
    }

    case 3:{
      float ans = compute(&f3, a, b, n, intensity, num_threads, granularity);
      std::cout << ans << "\n";
      break;
    }

    case 4:{
      float ans = compute(&f4, a, b, n, intensity, num_threads, granularity);
      std::cout << ans << "\n";
      break;
    }
    default:
    std::cout << "Wrong function ID";
  }

  auto end = std::chrono::steady_clock::now();
  double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e9;

  std::cerr << time_taken;
  return 0;
}
