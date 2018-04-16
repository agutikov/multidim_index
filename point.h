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

using el_type = double;

class point : public std::vector<el_type>
{
public:
  point() = default;
  point(size_t count, el_type val = el_type()) : std::vector<el_type>(count, val) {}
  point(std::initializer_list<el_type> init) : std::vector<el_type>(init) {}


  point& operator+=(const point& p)
  {
    std::transform(begin(), end(), p.begin(), begin(), std::plus<el_type>());
    return *this;
  }

  point& operator-=(const point& p)
  {
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
    return p;
  }

  friend point operator/(point p, el_type val)
  {
    std::transform(p.begin(), p.end(), p.begin(),
		   [val] (el_type v) -> el_type { return v/val; });
    return p;
  }

  friend point operator+(point p1, const point& p2)
  {
    std::transform(p1.begin(), p1.end(), p2.begin(), p1.begin(), std::plus<el_type>());
    return p1;
  }

  friend point operator-(point p1, const point& p2)
  {
    std::transform(p1.begin(), p1.end(), p2.begin(), p1.begin(), std::minus<el_type>());
    return p1;
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
    return std::inner_product(begin(), end(), begin(), 0);
  }
  el_type abs() const
  {
    return std::sqrt(l2());
  }

  el_type distance(const point& p) const
  {
    return std::sqrt(std::inner_product(begin(), end(), p.begin(), 0,
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

#endif
