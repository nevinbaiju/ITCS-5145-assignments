#include <iostream>
#include <thread>

void print_minion(int minion_id){
  std::cout << "Hello! I am minion " << minion_id << "\n";
}

int main (int argc, char** argv) {
  if (argc < 2) {
    std::cerr<<"usage: "<<argv[0]<<" <nbminions>\n";
    return -1;
  }
  
  int minions = atoi(argv[1]);
  std::thread minionThreads[minions];
  
  for (int i=1; i<=minions; i++){
    minionThreads[i-1] = std::thread(print_minion, i);
  }

  for (int i=1; i<=minions; i++){
    minionThreads[i-1].join();
  } 

  std::cout << "Hello Minions! I am the Overlord!\n";

  
  return 0;
}
