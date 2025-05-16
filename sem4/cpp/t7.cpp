#include <algorithm>
#include <cstddef>
#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

template <typename T> class SetImpl {
public:
  virtual void add(T element) = 0;
  virtual void remove(T element) = 0;
  virtual bool contains(const T &element) const = 0;
  virtual std::size_t size() const = 0;

  virtual void copyTo(SetImpl<T> *other) const = 0;
  virtual SetImpl<T> *intersect(SetImpl<T> *other) const = 0;
  virtual std::string getName() const = 0;

  virtual ~SetImpl() {};
};

template <typename T> class VectorSetImpl : public SetImpl<T> {
public:
  void add(T element) override {
    if (!contains(element)) {
      elements.push_back(element);
    }
  }
  void remove(T element) override {
    auto i = std::find(elements.begin(), elements.end(), element);
    elements.erase(i);
  }
  bool contains(const T &element) const override {
    return std::find(elements.begin(), elements.end(), element) !=
           elements.end();
  }
  std::size_t size() const override { return elements.size(); }

  void copyTo(SetImpl<T> *other) const override {
    for (auto element : elements) {
      other->add(element);
    }
  }
  SetImpl<T> *intersect(SetImpl<T> *other) const override {
    auto newSetImpl = new VectorSetImpl<T>();
    for (auto element : elements) {
      if (other->contains(element)) {
        newSetImpl->add(element);
      }
    }
    return newSetImpl;
  }

  std::string getName() const override { return std::string("VectorImpl"); }

private:
  std::vector<T> elements;
};

template <typename T> class TreeSetImpl : public SetImpl<T> {
public:
  void add(T element) override { elements.insert(element); }
  void remove(T element) override {
    auto i = std::find(elements.begin(), elements.end(), element);
    elements.erase(i);
  }
  bool contains(const T &element) const override {
    return elements.count(element) != 0;
  }
  std::size_t size() const override { return elements.size(); }

  void copyTo(SetImpl<T> *other) const override {
    for (auto element : elements) {
      other->add(element);
    }
  }
  SetImpl<T> *intersect(SetImpl<T> *other) const override {
    auto newSetImpl = new VectorSetImpl<T>();
    for (auto element : elements) {
      if (!other->contains(element)) {
        newSetImpl->add(element);
      }
    }
    return newSetImpl;
  }

  std::string getName() const override { return std::string("TreeImpl"); }

private:
  std::set<T> elements;
};

template <typename T> class Set {
public:
  void add(T element) {
    impl->add(element);
    checkResize(1);
  }
  void remove(T element) {
    impl->remove(element);
    checkResize(-1);
  }
  bool contains(const T &element) const { return impl->contains(element); }
  std::size_t size() const { return impl->size(); }

  std::string getName() { return impl->getName(); }

  Set<T> merge(const Set<T> &other) {
    Set<T> newSet{};
    auto newSize = size() + other.size();
    newSet.checkResize(newSize);

    impl->copyTo(newSet.impl.get());
    other.impl->copyTo(newSet.impl.get());
    return newSet;
  }
  Set<T> intersect(const Set<T> &other) {
    Set<T> newSet{};
    newSet.impl.reset(impl->intersect(other.impl.get()));
    return newSet;
  }

private:
  static std::size_t maxSize;
  std::unique_ptr<SetImpl<T>> impl{new VectorSetImpl<T>()};

  void checkResize(int delta) {
    auto s = size();
    if (delta > 0 && s != maxSize + 1 || delta < 0 && s != maxSize) {
      return;
    }
    std::unique_ptr<SetImpl<T>> newImpl;
    if (delta > 0) {
      newImpl = std::make_unique<TreeSetImpl<T>>();
    } else if (delta < 0) {
      newImpl = std::make_unique<VectorSetImpl<T>>();
    }
    impl->copyTo(newImpl.get());
    impl.reset(newImpl.release());
  }
};

template <typename T> std::size_t Set<T>::maxSize{2};

int main() {
  Set<double> set1{};

  set1.add(3);
  set1.add(5);
  std::cout << "With size " << set1.size() << ":" << set1.getName() << "\n";
  set1.add(2);
  std::cout << "With size " << set1.size() << ":" << set1.getName() << "\n";
  set1.remove(5);
  std::cout << "With size " << set1.size() << ":" << set1.getName() << "\n";

  Set<double> set2{};
  set2.add(1);
  set2.add(2);
  std::cout << "Merge size:" << set1.merge(set2).size() << "\n";
  std::cout << "Intersect size:" << set1.intersect(set2).size() << "\n";
  return 0;
}
