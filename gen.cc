// generate csv with N lines (vectors) of M doubles

#include <cstdio>
#include <bitset>
#include <iostream>
#include <cstdint>
#include <ctime>
#include <cstdlib>

#include "point.h"

int main(int argc, const char* argv[])
{
  std::srand(std::time(nullptr));
/*
  point p1 = {1, 2, 3};
  point p2 = {0, 1, 0};

  std::cout << p1 << std::endl;
  std::cout << p2 << std::endl;
  p2 += p1;
  std::cout << p2 << std::endl;
  std::cout << p1 - p2 << std::endl;
  std::cout << p1*2 << std::endl;
  std::cout << (p1 + p2)/2 << std::endl;

  std::cout << point({0, 0, 1}).abs() << std::endl;
  std::cout << point({1, 1, 1}).abs() << std::endl;

  std::cout << -p1 << std::endl;

  std::cout << p1 << "; " << p2 << std::endl;
  std::cout << p1.distance(p2) << std::endl;
  std::cout << (p1 - p2).abs() << std::endl;

  std::cout << point::random(128) << std::endl;
*/

  if (argc != 3) {
    std::cout << "\n  Usage: " << argv[0] << " <num_lines> <num_cols>\n" << std::endl;
  }

  size_t lines = std::strtol(argv[1], nullptr, 10);
  size_t cols = std::strtol(argv[2], nullptr, 10);

  for (size_t i = 0; i < lines; i++) {
    std::cout << point::random(cols) << std::endl;
  }

  return 0;
}
