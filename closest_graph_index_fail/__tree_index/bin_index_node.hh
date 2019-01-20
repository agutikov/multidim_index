
#include "point.hh"
#include <memory>


class node : public std::enable_shared_from_this<node>
{
public:
  size_t weight;
  point center;
  std::weak_ptr<node> parent;
  std::shared_ptr<node> child[2];

  node() :
    weight(1)
  {}

  node(const point& p) :
    weight(1),
    center(p)
  {}

  static std::shared_ptr<node> make_new_parent(std::shared_ptr<node> n1, std::shared_ptr<node> n2)
  {
    auto n = std::make_shared<node>();
    n->weight = n1->weight + n2->weight;
    n->center.set_center_of(n1->center, n1->weight, n2->center, n2->weight);
    n->child[0] = n1;
    n->child[1] = n2;
    n1->parent = n;
    n2->parent = n;
    return n;
  }

  std::shared_ptr<node> split_insert(std::shared_ptr<node> n)
  {
    assert(n);

    auto p = parent.lock();
    auto new_parent = node::make_new_parent(n, shared_from_this());
    if (p) {
      new_parent->parent = p;
      if (p->child[0].get() == this) {
        p->child[0] = new_parent;
      } else if (p->child[1].get() == this) {
        p->child[1] = new_parent;
      } else {
        assert(false);
      }
    }
    return new_parent;
  }

  void update()
  {
    weight = child[0]->weight + child[1]->weight;
    center.set_center_of(child[0]->center, child[0]->weight, child[1]->center, child[1]->weight);
  }

  std::pair<std::shared_ptr<node>, double> get_nearest_child(std::shared_ptr<node> n)
  {
    assert(n);
    assert(child[0]);
    assert(child[1]);

    double d0 = child[0]->distance(n);
    double d1 = child[1]->distance(n);
    if (d0 < d1) {
      return std::make_pair(child[0], d0);
    } else {
      return std::make_pair(child[1], d1);
    }
  }

  double distance(std::shared_ptr<node> n) const
  {
    assert(n);

    return center.distance(n->center);
  }

  double child_distance() const
  {
    assert(child[0]);
    assert(child[1]);

    return child[0]->distance(child[1]);
  }

  friend std::ostream& operator<< (std::ostream& os, const node& n)
  {
    //  os << &n << ' ' << n.weight << " (" << n.center << ") " << n.parent.lock().get() << " [" << n.child[0].get() << " " << n.child[1].get() << "]";
    os << n.weight << " (" << n.center << ") ";
    return os;
  }
};

std::pair<std::shared_ptr<node>, double> search_nearest(std::shared_ptr<node> root, std::shared_ptr<node> new_node)
{
  assert(root);
  assert(new_node);

  auto nearest = std::make_pair(root, root->distance(new_node));
  while (nearest.first->weight > 1) {
    nearest = nearest.first->get_nearest_child(new_node);
  }
  return nearest;
}

std::shared_ptr<node> update_parents(std::shared_ptr<node> current_node)
{
  assert(current_node);

  while (current_node->parent.lock()) {
    current_node = current_node->parent.lock();
    current_node->update();
  }
  return current_node;
}

std::shared_ptr<node> insert_point(std::shared_ptr<node> root, const point& p)
{
  auto new_node = std::make_shared<node>(p);
  if (!root) {
    return new_node;
  }

  auto nearest = search_nearest(root, new_node);
  auto nearest_node = nearest.first;

  double d = nearest.second;
  auto parent = nearest_node->parent.lock();
  if (parent && parent->weight == 2) {
    if (parent->child_distance() < d) {
      nearest_node = parent;
    }
  }

  auto new_parent = nearest_node->split_insert(new_node);
  return update_parents(new_parent);
}

void print_tree(std::shared_ptr<node> root, size_t offset = 0)
{
  for (size_t i = 0; i < offset; i++) {
    std::cout << ' ';
  }
  std::cout << *root << std::endl;
  if (root->child[0]) {
    print_tree(root->child[0], offset + 2);
  }
  if (root->child[1]) {
    print_tree(root->child[1], offset + 2);
  }
}
