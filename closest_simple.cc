// simple compare each
// 1 - find the closest point
// 2 - find K closest points
// 3 - find all closer than D

#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <ctime>
#include <fstream>

#include "point.h"

std::deque<point> from_csv_file(const std::string& filename)
{
  std::deque<point> points;

  std::ifstream file(filename, std::ios::in);

  point p;
  while (!file.eof()) {
    file >> p;
    if (p.size() > 0) {
      points.push_back(p);
    }
  }

  return std::move(points);
}

std::pair<double, point> find_closest(const std::deque<point>& where, const point& what)
{
  double min_d = std::numeric_limits<double>::max();
  const point* closest_p = nullptr;
  for (const auto& p : where) {
    double d = what.distance(p);
    if (d < min_d) {
      closest_p = &p;
      min_d = d;
    }
  }
  return std::make_pair(min_d, *closest_p);
}

int main(int argc, const char* argv[])
{
  if (argc != 3) {
    std::cout << "\n  Usage: " << argv[0] << " <csv_where_find> <csv_what_find>\n" << std::endl;
    return -1;
  }

  std::deque<point> points = from_csv_file(argv[1]);
  std::deque<point> points_to_search = from_csv_file(argv[2]);

  for (const auto& p : points_to_search) {
    auto res = find_closest(points, p);
    std::cout << p << std::endl << res.second << std::endl << res.first << std::endl << std::endl;
  }
  return 0;
}
