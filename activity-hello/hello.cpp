#include <unistd.h> 
#include <iostream>

int main () {
  char hostname[100];
  int result = gethostname(hostname, 100);
  std::cout << hostname;
  return 0;
}
