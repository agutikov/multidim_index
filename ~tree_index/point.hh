
#include <array>
#include <vector>

#define DIMENSIONS 2


template <class T, std::size_t N>
std::ostream& operator<<(std::ostream& o, const std::array<T, N>& arr)
{
  std::copy(arr.begin(), arr.end(), std::ostream_iterator<T>(o, " "));
  return o;
}


class point
{
public:
  point(const point& p) : x(p.x) {}

  point() = default;

  point(double v)
  {
    x.fill(v);
  }

  void randomize()
  {
    for (auto& x1 : x) {
      double v = std::rand();
      x1 = v / RAND_MAX;
    }
  }

  double distance(const point& p) const
  {
    double sum = 0.0;
    auto it = x.begin();
    auto it1 = p.x.begin();
    for (; it < x.end() && it1 < p.x.end(); ++it, ++it1) {
      register double x1 = *it;
      register double x2 = *it1;
      x1 -= x2;
      x1 *= x1;
      sum += x1;
    }
    return std::sqrt(sum);
  }

  void set_center_of(const point& p1, size_t weight1, const point& p2, size_t weight2)
  {
    size_t w = weight1 + weight2;
    auto it = x.begin();
    auto it1 = p1.x.begin();
    auto it2 = p2.x.begin();
    for (; it < x.end() && it1 < p1.x.end() && it2 < p2.x.end(); ++it, ++it1, ++it2) {
      *it = (*it1 * weight1 + *it2 * weight2) / w;
    }
  }

  friend bool operator==(const point& lhs, const point& rhs)
  {
    auto it1 = lhs.x.begin();
    auto it2 = rhs.x.begin();
    for (; it1 < lhs.x.end() && it2 < rhs.x.end(); ++it1, ++it2) {
      if (*it1 != *it2) {
        return false;
      }
    }
    return true;
  }

  friend std::ostream& operator<< (std::ostream& os, const point& p)
  {
    os << p.x;
    return os;
  }

private:
  std::array<double, DIMENSIONS> x;
};

void point_foo()
{
  point p1(0.5);
  point p2;
  point p3(p1);
  point p4;
  p4.randomize();

  std::cout << p1 << std::endl;
  std::cout << p2 << std::endl;
  std::cout << p3 << std::endl;
  std::cout << p4 << std::endl;

  std::cout << p2.distance(p1) << std::endl;
  std::cout << p2.distance(p2) << std::endl;
  std::cout << p2.distance(p3) << std::endl;
  std::cout << p2.distance(p4) << std::endl;

  point p5(1);
  std::cout << p5 << std::endl;

  p4.set_center_of(p2, 1, p5, 1);
  std::cout << p4 << std::endl;

  p4.set_center_of(p2, 1, p5, 5);
  std::cout << p4 << std::endl;

}
