
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <deque>
#include <ctime>
#include <iostream>

#include "point.h"


struct node
{
  struct dimention_closest_node
  {
    const node* _node;
    double distance;
    dimention_closest_node() :
      _node(nullptr),
      distance(-1)
    {}
    bool exists() const { return distance > 0; }

    friend std::ostream& operator<<(std::ostream& os, const dimention_closest_node& n) {
      if (n.exists()) {
        os << n._node->id << ", " << n.distance;
      } else {
        os << "null, -1";
      }
      return os;
    }
  };

  int id;
  const point* _point;
  // 0 - negative direction
  // 1 - positive direction
  std::vector<std::array<dimention_closest_node, 2>> closest_nodes;

  node(const point* p) :
    _point(p),
    closest_nodes(p->size())
  {
    static int id_counter = 1;
    id = id_counter;
    id_counter++;
  }

  friend std::ostream& operator<<(std::ostream& os, const node& n) {
    os << n.id << std::endl << *n._point << std::endl;
    for (const auto& d : n.closest_nodes) {
      for (const auto& cn : d) {
        os << cn << "; ";
      }
      os << std::endl;
    }
    return os;
  }
};

std::deque<node> build_index(const std::deque<point>& points)
{
  std::deque<node> nodes;
  for (const auto& p : points) {
    nodes.emplace_back(&p);
  }

  for (auto& a : nodes) {
    for (const auto& b : nodes) {
      if (a.id == b.id) {
        continue;
      }
      point v = *b._point - *a._point;
      double d = v.abs();

      int i_v = 0;
      for (auto j : v) {
        int i = -1;
        if (j < 0.0) {
          i = 0;
        } else if (j > 0.0) {
          i = 1;
        }

        if (i >= 0) {
          node::dimention_closest_node& cn = a.closest_nodes[i_v][i];
          if (!cn.exists() || d < cn.distance) {
            cn.distance = d;
            cn._node = &b;
          }
        }
        i_v++;
      }
    }
    //std::cout << a << std::endl;
  }

  return std::move(nodes);
}

std::pair<double, const node&> find_closest(const std::deque<node>& nodes, const point& p)
{
  const node* current = &nodes[0];
  point v = p - *current->_point;
  double d = v.abs();

  std::cerr << *current->_point << std::endl;

  for (;;) {
    const node* next = nullptr;

    int i_v = 0;
    for (const auto& cn : current->closest_nodes) {
      for (const auto& dir : cn) {
        double d1 = dir._node->_point->distance(p);
        if (d1 < d) {
          d = d1;
          next = dir._node;
        }
      }
      i_v++;
    }

    if (next == nullptr) {
      break;
    } else {
      current = next;
      std::cerr << *current->_point << std::endl;
      v = p - *current->_point;
    }
  }

  return std::make_pair(d, *current);
}

int main(int argc, const char* argv[])
{
  if (argc != 3) {
    std::cout << "\n  Usage: " << argv[0] << " <csv_where_find> <csv_what_find>\n" << std::endl;
    return -1;
  }

  std::deque<point> points = from_csv_file(argv[1]);
  std::deque<point> points_to_search = from_csv_file(argv[2]);

  std::deque<node> nodes = build_index(points);

  for (const auto& p : points_to_search) {
    std::cout << p << std::endl;
    auto r = find_closest(nodes, p);
    std::cout << *r.second._point << std::endl;
    std::cout << r.first << std::endl << std::endl;
  }




  /*
  std::vector<std::pair<const point*, std::vector<double>>> distances;
  for (const auto& p : points) {
    distances.push_back(std::make_pair(&p, std::move(std::vector<double>(points.size(), 0.0))));
  }
  int i_a = 0;
  for (auto& a : distances) {
    int i_b = 0;
    for (auto& b : distances) {
      if (a.second[i_b] == 0) {
        double d = a.first->distance(*b.first);
        a.second[i_b] = b.second[i_a] = d;
      }
      i_b++;
    }
    i_a++;
  }

  for (const auto& a : distances) {
    for (double d : a.second) {
      std::cout << d << ' ';
    }
    std::cout << std::endl;
  }
  */

  return 0;
}
