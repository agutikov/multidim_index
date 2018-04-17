#ifndef __POINT_H__
#define __POINT_H__

#include <vector>
#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <experimental/iterator>
#include <numeric>
#include <cmath>
#include <cstdlib>
#include <string>
#include <sstream>
#include <deque>
#include <fstream>
#include <stdexcept>

using el_type = double;

class point : public std::vector<el_type>
{
public:
  point() = default;
  point(size_t count, el_type val = el_type()) : std::vector<el_type>(count, val) {}
  point(std::initializer_list<el_type> init) : std::vector<el_type>(init) {}


  point& operator+=(const point& p)
  {
    check_dims(p);
    std::transform(begin(), end(), p.begin(), begin(), std::plus<el_type>());
    return *this;
  }

  point& operator-=(const point& p)
  {
    check_dims(p);
    std::transform(begin(), end(), p.begin(), begin(), std::minus<el_type>());
    return *this;
  }

  point operator-() const
  {
    point p(size());
    std::transform(begin(), end(), p.begin(), std::negate<el_type>());
    return std::move(p);
  }

  point& operator*=(el_type val)
  {
    std::transform(begin(), end(), begin(),
		   [val] (el_type v) -> el_type { return v*val; });
    return *this;
  }

  point& operator/=(el_type val)
  {
    std::transform(begin(), end(), begin(),
		   [val] (el_type v) -> el_type { return v/val; });
    return *this;
  }

  friend point operator*(point p, el_type val)
  {
    std::transform(p.begin(), p.end(), p.begin(),
		   [val] (el_type v) -> el_type { return v*val; });
    return std::move(p);
  }

  friend point operator/(point p, el_type val)
  {
    std::transform(p.begin(), p.end(), p.begin(),
		   [val] (el_type v) -> el_type { return v/val; });
    return std::move(p);
  }

  friend point operator+(point p1, const point& p2)
  {
    p1.check_dims(p2);
    std::transform(p1.begin(), p1.end(), p2.begin(), p1.begin(), std::plus<el_type>());
    return std::move(p1);
  }

  friend point operator-(point p1, const point& p2)
  {
    p1.check_dims(p2);
    std::transform(p1.begin(), p1.end(), p2.begin(), p1.begin(), std::minus<el_type>());
    return std::move(p1);
  }

  friend std::ostream& operator<<(std::ostream& os, const point& p)
  {
    std::copy(p.begin(), p.end(), std::experimental::make_ostream_joiner(os, ", "));
    return os;
  }

  friend std::istream& operator>>(std::istream& is, point& p)
  {
    p.clear();
    el_type el;
    char c;
    std::string line;
    if (std::getline(is, line)) {
      p = std::move(from_string(line));
    }
    return is;
  }

  static point from_string(const std::string& s)
  {
    el_type el;
    char c;
    point p;
    std::istringstream iss(s);
    for (;;) {
      iss >> el;
      p.push_back(el);
      do {
        iss >> c;
        if (iss.eof()) {
          return std::move(p);
        }
      } while (c != ',');
    }
    return std::move(p);
  }

  el_type l2() const
  {
    return std::inner_product(begin(), end(), begin(), (el_type)0);
  }
  el_type abs() const
  {
    return std::sqrt(l2());
  }

  void check_dims(const point& p) const
  {
    if (p.size() != size()) {
      std::stringstream ss;
      ss << "ERROR: different dimentions " << size() << " != " << p.size();
      throw std::length_error(ss.str());
    }
  }

  el_type distance(const point& p) const
  {
    check_dims(p);
    return std::sqrt(std::inner_product(begin(), end(), p.begin(), (el_type)0,
                      std::plus<el_type>(),
                      [] (el_type x1, el_type x2) -> el_type { el_type tmp = x1 - x2; return tmp*tmp; }));
  }

  static point random(size_t size)
  {
    point p(size);
    std::transform(p.begin(), p.end(), p.begin(), [](el_type)->el_type{return ((el_type)std::rand()) / RAND_MAX;});
    return std::move(p);
  }

};

std::deque<point> from_csv_file(const std::string& filename)
{
  std::deque<point> points;

  std::ifstream file(filename, std::ios::in);

  size_t vector_size = 0;
  size_t count = 0;
  point p;
  while (!file.eof()) {
    file >> p;
    count++;
    if (p.size() > 0) {
      if (vector_size == 0) {
        vector_size = p.size();
      } else if (vector_size != p.size()) {
        std::stringstream ss;
        ss << "ERROR: invalid vector size, all vectors must be same size, file \"" << filename << "\", line " << count;
        throw std::length_error(ss.str());
      }
      points.push_back(p);
    }
  }

  return std::move(points);
}

#endif
