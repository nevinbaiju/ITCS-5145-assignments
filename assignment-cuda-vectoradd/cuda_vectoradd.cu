#include <iostream>
#include <chrono>

__global__ void addVectors(int* A, int* B, int* C, int size) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < size) {
        C[i] = A[i] + B[i];
    }
}

int main (int argc, char* argv[]) {
  //TODO: add usage
  
  int n = atoi(argv[1]); //TODO: atoi is an unsafe function

  float* h_in1 = new float[n];
  float* h_in2 = new float[n];
  float* h_out = new float[n];
  
  
  for (int i=0; i<n; ++i) {
    h_in1[i] = i;
    h_in2[i] = 100+i;
  }


  
  std::chrono::time_point<std::chrono::system_clock> begin, end;
  begin = std::chrono::system_clock::now();

  // Allocate memory on the device
  int* d_in1, * d_in2, * d_out;
  cudaMalloc(&d_in1, n * sizeof(int));
  cudaMalloc(&d_in2, n * sizeof(int));
  cudaMalloc(&d_out, n * sizeof(int));

  // Copy vectors from host to device
  cudaMemcpy(d_in1, h_in1, n * sizeof(int), cudaMemcpyHostToDevice);
  cudaMemcpy(d_in2, h_in2, n * sizeof(int), cudaMemcpyHostToDevice);

  // Launch kernel to add vectors
  int threadsPerBlock = 256;
  int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
  addVectors<<<blocksPerGrid, threadsPerBlock>>>(d_in1, d_in2, d_out, n);

  // Copy result vector from device to host
  cudaMemcpy(h_out, d_out, n * sizeof(int), cudaMemcpyDeviceToHost);

  
  end = std::chrono::system_clock::now();
  std::chrono::duration<double> totaltime = (end-begin);

  std::cout<<n<<" "<<totaltime.count()<<std::endl;

  delete[] h_in1;
  delete[] h_in2;
  delete[] h_out;

  cudaFree(d_in1);
  cudaFree(d_in2);
  cudaFree(d_out);

  return 0;
}
