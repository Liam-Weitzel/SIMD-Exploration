#include <iostream>

int main (int argc, char *argv[]) {
  float evens[] = {2.0, 4.0, 6.0, 8.0, 10.0, 12.0, 14.0, 16.0};
  float odds[] = {1.0, 3.0, 5.0, 7.0, 9.0, 11.0, 13.0, 15.0};

  float result[8];
  for(int i = 0; i < 8; ++i) {
    result[i] = evens[i]-odds[i];
  }

  std::cout << "Result: ";
  for(int i = 0; i < 8; ++i) {
    std::cout << result[i] << " ";
  }
  std::cout << std::endl;

  return 0;
}
