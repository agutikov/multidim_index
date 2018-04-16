
// read text from stdin, shuffle lines and print to stdout

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <ctime>

#include "point.h"

int main(int argc, const char* argv[])
{
  std::srand(std::time(nullptr));

  std::deque<point> points;

  point p;
  while (!std::cin.eof()) {
    std::cin >> p;
    if (p.size() > 0) {
      points.push_back(p);
    }
    //std::cout << "in: " << p << std::endl;
  }

  while (points.size() > 0) {
    //std::cout << "out: " << points.size() << std::endl;
    size_t i = std::rand() / (RAND_MAX / points.size());
    std::cout << points[i] << std::endl;
    points.erase(points.begin() + i);
  }

  return 0;
}
