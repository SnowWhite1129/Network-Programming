#include <iostream>
#include <cmath>
#include <cstdlib>
using namespace std;
int main(int argc, char* argv[]) {
  if (argc == 2) {
    int n = atoi(argv[1]);
    exit(n * n);
  }
  exit(0);
}
