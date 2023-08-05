#ifndef SRC_CONTAINERS_S21_LIST_H_
#define SRC_CONTAINERS_S21_LIST_H_

#include <cstdint>
#include <initializer_list>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace dlwhi {

using size_t = int64_t;

template <typename T, typename Allocator = std::allocator<T>>
class list {
  template <typename U>
  struct Node;

  using node_type = Node<T>;
  using node_ptr = Node<T>::pointer;
  using rebind_alloc = typename std::allocator_traits<Allocator>::template rebind_alloc<node_type>;
 public:
  template <typename U>
  class ListIterator;

  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using move_reference = T&&;

  using allocator_type = rebind_alloc;
  using al_traits = std::allocator_traits<allocator_type>;

  // using iterator = dlwhi::iterator<T, vector>;
  // using const_iterator = dlwhi::iterator<const T, vector>;
  // using reverse_iterator = dlwhi::reverse_iterator<iterator>;
  // using const_reverse_iterator = dlwhi::reverse_iterator<const_iterator>;

  list() : al_(Allocator()), head_(al_.allocate(1)), node_c_(0){
    head_->construct_head();
  };

  explicit list(Allocator& alloc) : al_(alloc), head_(al_.allocate(1)), node_c_(0){
    head_->construct_head();
  };

  list(size_t count, const_reference val, Allocator& alloc = Allocator()) 
      : al_(alloc), head_(al_.allocate(1)), node_c_(count){
    node_ptr ptr = head_->construct_head();
    for (size_t i = 0; i < node_c_; ++i, ptr = ptr->next()) {
      node_ptr some = al_.allocate(1);
      node_type::construct(some, ptr, head_, val);
      some->bind(ptr, head_);
    }
  };

  /*  Parametarised constructor
  in stl list is circular */
  // list(size_type n) : list() {
  //   Node* ptr = &head_;
  //   for (size_type i = 0; i < n; ++i) {
  //     Node* some = new Node{0, ptr, &head_};
  //     ptr->next_ = some;
  //     head_.prev_ = some;
  //     ptr = ptr->next_;
  //   }
  //   counter_ = n;
  // }

  //  Copy constructor:
  // list(const list& l) : list() {
  //   const Node* ptrToL = l.head_.next_;
  //   Node* ptrToCopy = &head_;
  //   for (size_type i = 0; i < l.counter_; ++i) {
  //     Node* some = new Node{ptrToL->data_, ptrToCopy, &head_};
  //     ptrToCopy->next_ = some;
  //     head_.prev_ = some;
  //     ptrToCopy = ptrToCopy->next_;
  //     ptrToL = ptrToL->next_;
  //   }
  //   counter_ = l.counter_;
  // }
  // //  Move constructor (no copying for anything but
  // //  primitive types and rvalue is undefiened but consistent)
  // list(list&& l) : list() {
  //   //  using 'std::move' to make othar a rvalue and make
  //   //  compilator to use move not copy
  //   *this = std::move(l);
  // }
  // //  initializer list constructor, creates a list initizialized using
  // //  std::initializer_list<T>
  // list(std::initializer_list<value_type> const& items) : list() {
  //   Node* ptr = &head_;
  //   // items - initiliser list
  //   //  we create i as an iterator to work with initiliser list
  //   for (auto i = items.begin(); i != items.end(); ++i) {
  //     //  *i  get us value stored in a parrticular element of the initiliser
  //     //  list
  //     Node* some = new Node{*i, ptr, &head_};
  //     head_.prev_ = some;
  //     ptr->next_ = some;
  //     ptr = ptr->next_;
  //   }
  //   counter_ = items.size();
  // }

  // //  assignment operator overload for moving an object
  // list& operator=(list&& l) {
  //   std::swap(head_, l.head_);
  //   counter_ = l.counter_;
  //   l.counter_ = 0;
  //   return *this;
  // }

  // //  assignment operator overload for copying an object
  // list& operator=(list& l) {
  //   list<T> cpy(l);
  //   swap(cpy);
  //   return *this;
  // }

  // //  destructor
  // ~list() {
  //   Node* ptr = end().ptr_->prev_;
  //   for (size_t i = 0; i < counter_; ++i) {
  //     Node* tmp = ptr->prev_;
  //     delete ptr;
  //     ptr = tmp;
  //   }
  // }

  // //  List Element access:---------------------------------------------------
  // reference front() { return *begin(); }
  // reference back() { return *(end() - 1); }
  
  // const_reference front() const { return *begin(); }
  // const_reference back() const { return *(end() - 1); }

  
  // iterator begin() { return iterator(&head_) + 1; }
  // iterator end() { return iterator(&head_); }

  bool empty() const noexcept { return !node_c_;}
  size_t max_size() const noexcept { return al_traits::max_size(al_); }
  size_t size() const noexcept { return node_c_; }

  // //  void clear() {}
  // iterator insert(iterator pos, const_reference value) {
  //   Node* some = new Node{value, pos.ptr_->prev_, pos.ptr_};
  //   pos.ptr_->prev_->next_ = some;
  //   pos.ptr_->prev_ = some;
  //   counter_ += 1;
  //   return ListIterator(some);
  // }

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

  // //  For testing purposes:
  // int IsEqual(const list& other) {
  //   if (counter_ != other.counter_) {
  //     return 0;
  //   } 
  //   if (counter_ == 0) {
  //     return 1;
  //   }
  //   iterator first = begin();
  //   iterator second = other.head_.next_;
  //   while (first != end()) {
  //     if (*first != *second) {
  //       return 0;
  //     } 
  //     ++first;
  //     ++second;
  //   }
  //   return 1;
  // }

  // friend std::ostream& operator<<(std::ostream& os, list some) {
  //   Node* ptr = some.head_.next_;
  //   for (size_t i = 0; i < some.counter_; ++i) {
  //     std::cout << std::endl << ptr->data_ << std::endl;
  //     ptr = ptr->next_;
  //   }
  //   std::cout << std::endl;
  //   return os;
  // }

  // template <typename TI>
  // class ListIterator {
  //   /* A friend class can access private and protected members
  //   of other classes in which it is declared as a friend */

  //   friend list;

  //  public:
  //   //  Default constructor:
  //   ListIterator() : ptr_(nullptr) {}
  //   //  Parametarised constructor:
  //   ListIterator(TI* data) noexcept : ptr_(data){};
  //   // Copy constructor:
  //   ListIterator(const ListIterator& other) : ptr_(other.ptr_) {}

  //   // Assignment operator overload for copying an object
  //   ListIterator& operator=(
  //       const ListIterator& other) noexcept {  //  why noexept?
  //     ptr_ = other.ptr_;
  //     return *this;
  //   }
  //   ListIterator& operator=(iterator&& other) noexcept {
  //     ptr_ = other.ptr_;
  //     return *this;
  //   }
  //   //  Destructor
  //   ~ListIterator() = default;

  //   //  Operators overload:
  //   ListIterator& operator++() noexcept {
  //     ptr_ = ptr_->next_;
  //     return *this;
  //   }
  //   ListIterator& operator--() noexcept {
  //     ptr_ = ptr_->prev_;
  //     return *this;
  //   }

  //   bool operator==(const iterator other) { return ptr_ == other.ptr_; }

  //   bool operator!=(const iterator other) { return ptr_ != other.ptr_; }

  //   //  - int overload used in insert:
  //   ListIterator operator-(int k) {
  //     ListIterator tmp(ptr_);  //  parametarised constructor
  //     for (int i = 0; i < k; ++i) {
  //       tmp.ptr_ = tmp.ptr_->prev_;
  //     }
  //     return tmp;
  //   }

  //   ListIterator operator+(int k) {
  //     ListIterator tmp(ptr_);  //  parametarised constructor
  //     for (int i = 0; i < k; ++i) {
  //       tmp.ptr_ = tmp.ptr_->next_;
  //     }
  //     return tmp;
  //   }

  //   //  implicit conversion
  //   operator ListIterator<const TI>() const {
  //     return ListIterator<const TI>(ptr_);
  //   }

  //   TI& operator*() { return ptr_->data_; }

  //  private:
  //   TI* ptr_;
  // };

 private:
  template <typename U>
  struct Node {
    using value_type = U;
    using pointer = Node*;

    pointer construct(pointer prev = nullptr, pointer next = nullptr) {
      new(this) pointer(prev);
      new(reinterpret_cast<uint8_t*>(this) + sizeof(pointer)) pointer(next);
      return this;
    }
    pointer construct(U val) {
      new(reinterpret_cast<uint8_t*>(this) + 2*sizeof(pointer)) U(val);
      return this;
    }
    pointer construct(pointer prev, pointer next, U val) {
      construct(prev, next);
      return construct(val);
    }
  
    pointer construct_head() {
      this->construct();
      if constexpr (std::is_default_constructible<T>::value)
        this->construct(T());
      return this;
    }

    pointer next() { return next_;}
    pointer prev() { return prev_;}

    void bind(pointer lhs, pointer rhs) {
      lhs->next_ = this;
      rhs->prev_ = this;
    }

    pointer prev_;
    pointer next_;
    U data_;
  };

  rebind_alloc al_;
  size_t node_c_;
  node_ptr head_;
};

}  // namespace s21

#endif  // SRC_CONTAINERS_S21_LIST_H_
