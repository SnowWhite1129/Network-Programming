#include <iostream>
using namespace std;
int main() {
  int count = 0;
  char c;
  while (cin.get(c)) ++count;
  cout << count << endl;
  return 0;
}
