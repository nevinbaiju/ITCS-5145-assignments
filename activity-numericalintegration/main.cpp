#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>

#include <functional>
#include <time.h>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

using namespace std;

float f1(float x, int intensity);
float f2(float x, int intensity);
float f3(float x, int intensity);
float f4(float x, int intensity);

#ifdef __cplusplus
}
#endif


float compute (float (*func)(float, int), float a, float b, int n, int intensity){
  float b_minus_a_over_n = (b - a)/n;
  float ans = 0;
  for (int i = 0; i<n; i++){
    ans = ans + func(a + (i + 0.5)*b_minus_a_over_n, intensity);
  }
  ans = ans*b_minus_a_over_n;

  return ans;

}

int main (int argc, char* argv[]) {
  auto start = chrono::steady_clock::now();
  if (argc < 6) {
    std::cerr<<"usage: "<<argv[0]<<" <functionid> <a> <b> <n> <intensity>"<<std::endl;
    return -1;
  }

  int function_id = atoi(argv[1]);
  float a = atof(argv[2]);
  float b = atof(argv[3]);
  int n = atoi(argv[4]);
  int intensity = atoi(argv[5]);


  switch(function_id) {
    case 1:{
      float ans = compute(&f1, a, b, n, intensity);
      cout << ans << "\n";
      break;
    }

    case 2:{
      float ans = compute(&f2, a, b, n, intensity);
      cout << ans << "\n";
      break;
    }

    case 3:{
      float ans = compute(&f3, a, b, n, intensity);
      cout << ans << "\n";
      break;
    }

    case 4:{
      float ans = compute(&f4, a, b, n, intensity);
      cout << ans << "\n";
      break;
    }
    default:
    cout << "Wrong function ID";
  }

  auto end = chrono::steady_clock::now();
  double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1e9;

  cerr << time_taken;
  return 0;
}