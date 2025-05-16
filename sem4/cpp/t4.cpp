#include <cassert>
#include <iostream>

template <typename D> struct less_than_comparable {
  bool operator==(D const &rhs) const {
    return static_cast<const D *>(this)->value() == rhs.value();
  }

  bool operator<=(D const &rhs) const {
    return static_cast<const D *>(this)->value() <= rhs.value();
  }

  bool operator>(D const &rhs) const {
    return !static_cast<const D *>(this)->operator<=(rhs);
  }

  bool operator!=(D const &rhs) const {
    return !static_cast<const D *>(this)->operator==(rhs);
  }

  bool operator>=(D const &rhs) const {
    return !static_cast<const D *>(this)->operator<(rhs);
  }
};

template <typename D> class counter {
public:
  static size_t count;

protected:
  counter() { ++count; }
  counter(const counter &) { ++count; }
  ~counter() { --count; }
};

template <typename D> size_t counter<D>::count(0);

class Number : public less_than_comparable<Number>, public counter<Number> {
public:
  Number(int value) : m_value{value} {}
  int value() const { return m_value; }
  bool operator<(Number const &other) const { return m_value < other.m_value; }

private:
  int m_value;
};

int main() {
  Number one{1};
  Number two{2};
  Number three{3};
  Number four{4};
  assert(one >= one);
  assert(three <= four);
  assert(two == two);
  assert(three > two);
  assert(one < two);
  std::cout << "Count: " << counter<Number>::count << std::endl;
  return 0;
}
