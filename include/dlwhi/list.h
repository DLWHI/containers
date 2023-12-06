#ifndef SRC_CONTAINERS_S21_LIST_H_
#define SRC_CONTAINERS_S21_LIST_H_

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "node_iterator.h"
#include "reverse_iterator.h"

namespace sp {

using size_t = int64_t;

template <typename T, typename Allocator = std::allocator<T>>
class list {
  struct Node;

  using node_type = Node;
  using node_ptr = Node*;
  using rebind_alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<node_type>;
  using rebind_traits = std::allocator_traits<rebind_alloc>;
 public:
  template <typename U>
  class ListIterator;

  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using move_reference = T&&;
  using size_t = int64_t;

  using allocator_type = Allocator;
  using al_traits = std::allocator_traits<allocator_type>;

  using iterator = sp::node_iterator<node_type, list>;
  using const_iterator = sp::node_iterator<const node_type, list>;
  using reverse_iterator = sp::reverse_iterator<iterator>;
  using const_reverse_iterator = sp::reverse_iterator<const_iterator>;

  list() : al_(Allocator()), node_c_(0), head_(node_al_.allocate(1)){
    head_->construct_head(al_);
  };

  explicit list(const Allocator& alloc) : al_(alloc), node_c_(0), head_(node_al_.allocate(1)){
    head_->construct_head(al_);
  };

  list(size_t count, const_reference val, const Allocator& alloc = Allocator()) 
      : al_(alloc), node_c_(count), head_(node_al_.allocate(1)){
    head_->construct_head(al_);
    construct_multiple_nodes(head_, count, val);
  };

  list(size_t count, const Allocator& alloc = Allocator()) 
      : al_(alloc), node_c_(count), head_(node_al_.allocate(1)) {
    head_->construct_head(al_);
    construct_multiple_nodes(head_, count);
  }

  template <typename InputIterator>
  list(InputIterator start, InputIterator end, const Allocator& alloc = Allocator())
      : al_(alloc), node_c_(std::distance(start, end)), head_(node_al_.allocate(1)) {
    head_->construct_head(al_);
    construct_multiple_nodes(head_, start, end);
  }

  list(std::initializer_list<T> vals, const Allocator& alloc = Allocator()) 
      : list(vals.begin(), vals.end(), alloc) { }

  list(const list& other) : list(other.begin(), other.end(), other.al_) { }

  list(const list& other, const Allocator& alloc) : list(other.begin(), other.end(), alloc) { };

  list(list&& other) : list() {*this = std::move(other); }

  list(list&& other, const Allocator& alloc) : list(alloc) {
    std::swap(node_c_, other.node_c_);
    std::swap(head_, other.head_);
  }

  list& operator=(list& other) {
    list cpy(other);
    *this = std::move(cpy);
    return *this;
  }

  list& operator=(list&& other) {
    std::swap(node_c_, other.node_c_);
    std::swap(head_, other.head_);
    node_al_ = std::move(other.node_al_);
    al_ = std::move(other.al_);
    return *this;
  }

  virtual ~list() {
    destroy_multiple_nodes(head_->next(), head_);
    if constexpr (std::is_default_constructible<T>::value)
      al_traits::destroy(al_, &(head_->data));
    node_al_.deallocate(head_, 1);
  }

  // //  List Element access:---------------------------------------------------
  reference front() { return head_->next()->data; }
  reference back() { return head_->prev()->data; }
  
  const_reference front() const { return head_->next()->data; }
  const_reference back() const { return head_->prev()->data; }

  
  iterator begin() { return iterator(head_->next()); }
  const_iterator begin() const { return cbegin(); }
  const_iterator cbegin() const { return const_iterator(head_->next()); }

  iterator end() { return iterator(head_); }
  const_iterator end() const { return cend(); }
  const_iterator cend() const { return const_iterator(head_); }

  bool empty() const noexcept { return !node_c_;}
  size_t max_size() const noexcept { return al_traits::max_size(al_); }
  size_t size() const noexcept { return node_c_; }

   void clear() {
    destroy_multiple_nodes(head_->next(), head_);
    node_c_ = 0;
    head_->next_node = head_;
    head_->prev_node = head_;
   }

  iterator insert(const_iterator pos, const_reference value) {
    Node* some = node_al_.allocate(1);
    Node* target = const_cast<Node*>(pos.base());
    some->construct(target->prev_node, target, al_, value);
    some->bind(target->prev_node, target);
    node_c_ += 1;
    return iterator(some);
  }

  iterator insert(const_iterator pos, move_reference value) {
    Node* some = node_al_.allocate(1);
    Node* target = const_cast<Node*>(pos.base());
    some->construct(target->prev_node, target, al_, std::forward<value_type>(value));
    some->bind(target->prev_node, target);
    node_c_ += 1;
    return iterator(some);
  }

  iterator insert(const_iterator pos, size_t count, const_reference value );

  template<typename InputIterator>
  iterator insert(const_iterator start, InputIterator end, InputIterator last);

  iterator insert(const_iterator pos, std::initializer_list<T> vals);

  // //  Deletes an element at pos
  // void erase(iterator pos) {
  //   Node* nextNode = (pos + 1).ptr_;
  //   Node* prevNode = (pos - 1).ptr_;
  //   prevNode->next_ = nextNode;
  //   nextNode->prev_ = prevNode;
  //   delete pos.ptr_;
  //   counter_ -= 1;
  // }

  // void push_back(const_reference value) {
  //   //  insert puts a node at a position before the one iterator is pointing to
  //   insert(end(), value);
  // }

  // void pop_back() { erase(end() - 1); }

  // void push_front(const_reference value) { insert(begin(), value); }

  // void pop_front() { erase(begin()); }

  // void swap(list& other) {
  //   std::swap(counter_, other.counter_);
  //   std::swap(head_, other.head_);
  // }

  // void merge(list& other) {
  //   iterator iter = begin();
  //   iterator iter_other = other.begin();
  //   while (iter != end() && iter_other != other.end()) {
  //     if (*iter > *iter_other) {
  //       insert(iter, *iter_other);
  //       other.erase(iter_other);
  //       ++iter_other;
  //     } else {
  //       ++iter;
  //     }
  //   }
  // }

  // //  No elements are copied or moved, only the internal
  // //  pointers of the list nodes are re-pointed. Transfers all
  // //  elements from other into *this. The elements are inserted
  // //  before the element pointed to by pos. The container other
  // //  becomes empty after the operation.
  // void splice(const_iterator pos, list& other) {
  //   Node* nextNode = const_cast<Node*>((pos).ptr_);
  //   Node* prevNode = const_cast<Node*>((pos - 1).ptr_);
  //   prevNode->next_ = other.begin().ptr_;
  //   nextNode->prev_ = other.end().ptr_->prev_;
  //   other.head_.prev_ = &other.head_;
  //   other.head_.next_ = &other.head_;
  //   counter_ += other.counter_;
  //   other.counter_ = 0;
  // }
  // void reverse() {
  //   Node* ptr = head_.next_;
  //   for (size_t i = 0; i < counter_; ++i) {
  //     insert(ptr, back());
  //     pop_back();
  //   }
  // }

  // void unique() {
  //   iterator first = begin();
  //   iterator second = first + 1;
  //   while (second != end()) {
  //     if (*second == *first) {
  //       erase(second);
  //       second = first;
  //     } else {
  //       ++first;
  //     }
  //     ++second;
  //   }
  // }

  // // void sort() {
  // //   if (counter_ > 1) {
  // //       MergeSorting(&this);
  // //   }
  // // }

  // //  rename to sort

  // // void sort() {
  // //     if (counter_ < 2) {
  // //         return;
  // //       }
  // //     list a();
  // //     list b();
  // //     FrontBackSplit(*this, a, b);
  // //     a.sort();
  // //     b.sort();
  // //     *this = a.merge(b);
  // //     return;
  // // }

  // // void FrontBackSplit(list &initial, list &a, list &b) {
  // //       iterator middle = initial.begin();
  // //       size_t limit = (initial.counter_ % 2 > 0) ? initial.counter_ / 2 + 1 : initial.counter_ / 2;
  // //       size_t i = 0;
  // //       for (; i < limit; ++i) {
  // //           ++middle;
  // //           // middle = middle.ptr_->next;
  // //       }
  // //       b.head_.next_ = middle.ptr_;
  // //       b.head_.prev_ = initial.end().ptr_->prev_;
  // //       initial.end().ptr_->prev_.next_ = &b.head_;
  // //       b.counter_ = initial.counter_ / 2;

  // //       a.head_.next_ = initial.begin().ptr_;
  // //       a.head_.prev_ = middle.ptr_->prev_;
  // //       middle.ptr_->prev_.next_ = &a.head_;
  // //       initial.begin().ptr_->prev = &a.head_;
  // //       a.counter_ = i;

  // //       initial.head_.prev_ = initial.head_;
  // //       initial.head_.next_ = initial.head_.prev_;
  // //       initial.counter_ = 0;
  // // }

  bool operator==(const list& other) const {
    if (node_c_ != other.node_c_) {
      return false;
    } 
    const_iterator second = other.begin();
    for (auto first = begin(); first != end(); ++first) {
      if (*first != *second)
        return false;
    }
    return true;
  }

  bool operator!=(const list& other) const {
    return !(*this == other);
  }

  friend std::ostream& operator<<(std::ostream& os, const list& some) {
    const Node* ptr = some.head_->next_node;
    for (; ptr->next_node != some.head_; ptr = ptr->next_node) {
      os << ptr->data << ' ';
    }
    os << ptr->data;
    return os;
  }
 private:
  struct Node {
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using allocator_type = Allocator;
    using al_traits = std::allocator_traits<allocator_type>;

    Node* construct(Node* prev = nullptr, Node* next = nullptr) {
      new(&prev_node) Node*(prev);
      new(&next_node) Node*(next);
      return this;
    }

    template <typename... Args>
    Node* construct(allocator_type& al, Args&&... args) {
      al_traits::construct(al, &data, std::forward<Args>(args)...);
      return this;
    }

    template <typename... Args>
    Node* construct(Node* prev, Node* next, allocator_type& al, Args&&... args) {
      construct(prev, next);
      return construct(al, std::forward<Args>(args)...);
    }
  
    Node* construct_head(allocator_type& al) {
      construct(this, this);
      if constexpr (std::is_default_constructible<T>::value)
        construct(al);
      return this;
    }

    Node* next() const { return next_node;}
    Node* prev() const { return prev_node;}

    T& value() { return data;}
    const T& value() const { return data;}

    void bind(Node* lhs, Node* rhs) {
      lhs->next_node = this;
      rhs->prev_node = this;
    }

    // operator pointer_iterator<const T>() const noexcept {
    //   return pointer_iterator<const T>(const_cast<const T*>(ptr_));
    // }

    Node* prev_node;
    Node* next_node;
    T data;
  };

  template <typename... Args>
  node_ptr construct_multiple_nodes(node_ptr where, sp::size_t count, Args&&... args) {
    for (size_t i = 0; i < count; ++i, where = where->next()) {
      node_ptr some = node_al_.allocate(1);
      some->construct(where, head_, al_, std::forward<Args>(args)...);
      some->bind(where, head_);
    }
    return where;
  }

  template <typename InputIterator>
  node_ptr construct_multiple_nodes(node_ptr where, InputIterator start, InputIterator end) {
    for (; start != end; ++start, where = where->next()) {
      node_ptr some = node_al_.allocate(1);
      some->construct(where, head_, al_, *start);
      some->bind(where, head_);
    }
    return where;
  }

  void destroy_multiple_nodes(node_ptr start, node_ptr end) {
    node_ptr fwd = start;
    node_ptr bwd = start->next();
    while(fwd != end) {
      al_traits::destroy(al_, &(fwd->data));
      node_al_.deallocate(fwd, 1);
      fwd = bwd;
      bwd = bwd->next();
    }
  }

  rebind_alloc node_al_;
  allocator_type al_;
  size_t node_c_;
  node_ptr head_;
};

}  // namespace s21

#endif  // SRC_CONTAINERS_S21_LIST_H_
