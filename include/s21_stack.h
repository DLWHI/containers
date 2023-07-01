#ifndef SRC_CONTAINERS_S21_STACK_H_
#define SRC_CONTAINERS_S21_STACK_H_
#include "s21_list.h"

namespace s21 {
template <typename T, typename containers = s21::list<T>>

class stack {
 public:
  typedef T value_type;
  typedef T& reference;
  typedef const T& const_reference;
  typedef typename containers::iterator iterator;
  typedef typename containers::const_iterator const_iterator;
  typedef size_t size_type;

  //  Default constructor
  stack() : base() {}

  //  Initializer list constructor
  stack(std::initializer_list<value_type> const& items) : base(items) {}

  //  Copy 
  stack(const stack& other) : base(other.base) {}

  //  Move constructor
  stack(stack&& other) : base(std::move(other.base)) {}

  //  Assignment operator overload for moving an object
  stack& operator=(stack&& s) {
    base = std::move(s.base);
    return *this;
  }

  //  Destructor: 
  ~stack() = default;

//  Stack Element access:
const_reference top() {
  return base.back();
 }

// Stack Capacity:
bool empty() {
  return base.empty();
}

size_type size() {
  return base.size();
}

  void pop() { base.pop_back(); }
  void push(const_reference value) { base.push_back(value); }
  void swap(stack& other) { base.swap(other.base); }

 private:
  containers base;
};
}  // namespace s21
#endif  // SRC_CONTAINERS_S21_STACK_H_