#ifndef SRC_CONTAINERS_S21_VECTOR_H_
#define SRC_CONTAINERS_S21_VECTOR_H_
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "iterator.h"

namespace s21 {
typedef int64_t size_t;

template <typename T>
class allocator {
 public:
  T* allocate(size_t count) {
    return static_cast<T*>(operator new(count * sizeof(T)));
  }

  void deallocate(T* ptr_) { operator delete(ptr_); }

  void move_chunk_right(T* left, T* right, T* where) {
    where += right - left - 1;
    for (T* i = right - 1; i != left - 1; i--) {
      new (where--) T(move_if_possible(*i));
      i->~T();
    }
  }

  void move_chunk_left(T* left, T* right, T* where) {
    for (T* i = left; i != right; i++) {
      new (where++) T(move_if_possible(*i));
      i->~T();
    }
  }

  template <typename... Args>
  void construct(T* where, Args&&... args) {
    new (where) T(std::forward<Args>(args)...);
  }

  template <typename InputIterator>
  void construct(T* where, InputIterator left, InputIterator right) {
    for (; left != right; left++, where++) construct(where, *left);
  }

  void destroy(T* where) { where->~T(); }

  template <typename... Args>
  void construct_multiple(T* where, size_t count, Args&&... args) {
    for (size_t i = 0; i < count; i++, where++) construct(where, args...);
  }

  void destroy_multiple(T* where, size_t count) {
    for (size_t i = 0; i < count; i++, where++) destroy(where);
  }

 private:
  typename std::conditional<std::is_move_constructible<T>::value, T&&,
                            const T&>::type
  move_if_possible(T& what) {
    return std::move(what);
  }
};

// Requires T to be at least CopyConstructible and CopyASSignable
template <typename T>
class vector {
 public:
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T&& move_reference;
  typedef size_t size_type;
  typedef iterator<T, vector> v_iterator;
  typedef iterator<const T, vector> v_const_iterator;
  typedef reverse_iterator<T, vector> v_reverse_iterator;
  typedef reverse_iterator<const T, vector> v_creverse_iterator;

  // static constexpr double kCapMul = 1.5;
  static constexpr size_t kCapMul = 2;

  // Constructor list:
  //  1. default
  //  2. size
  //  3. size, value_to_copy
  //  4. iterator
  //  5. init_list
  //  6. copy
  //  7. move
  //  8. copy assignmet
  //  9. move assignmet
  //  10. destructor
  // Correct me if I forgot something
  vector() noexcept : size_(0), capacity_(0), ptr_(nullptr){};

  explicit vector(size_t size) : size_(size), capacity_(size_) {
    ptr_ = al.allocate(capacity_);
    al.construct_multiple(ptr_, size_);
  };

  explicit vector(size_t size, const_reference value)
      : size_(size), capacity_(size_) {
    ptr_ = al.allocate(capacity_);
    al.construct_multiple(ptr_, size_, value);
  };

  template <typename InputIterator>
  vector(const InputIterator& start, const InputIterator& end)
      : size_(std::distance(start, end)), capacity_(size_) {
    ptr_ = al.allocate(capacity_);
    al.construct(ptr_, start, end);
  }

  vector(std::initializer_list<value_type> values)
      : vector(values.begin(), values.end()){};

  vector(const vector& other) : vector(other.begin(), other.end()){};

  vector(vector&& other) noexcept : vector() { *this = std::move(other); };

  vector& operator=(const vector& other) {
    vector cpy(other);
    *this = std::move(cpy);
    return *this;
  }

  vector& operator=(vector&& other) noexcept {
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
    std::swap(ptr_, other.ptr_);
    return *this;
  }

  virtual ~vector() {
    al.destroy_multiple(ptr_, size_);
    al.deallocate(ptr_);
  }

  reference at(size_t pos) {
    if (pos >= size_)
      throw std::out_of_range("Accessing element out of bounds");
    return ptr_[pos];
  }

  const_reference at(size_t pos) const {
    if (pos >= size_)
      throw std::out_of_range("Accessing element out of bounds");
    return ptr_[pos];
  }

  reference front() noexcept { return ptr_[0]; }
  reference back() noexcept { return ptr_[size_ - 1]; }
  pointer data() noexcept { return ptr_; }

  const_reference front() const noexcept { return ptr_[0]; }
  const_reference back() const noexcept { return ptr_[size_ - 1]; }
  const_pointer data() const noexcept { return ptr_; }

  bool empty() const noexcept { return !size_; }
  size_t size() const noexcept { return size_; }
  size_t max_size() const noexcept {
    return std::numeric_limits<size_t>::max() / sizeof(value_type);
  }
  size_t capacity() const noexcept { return capacity_; }

  v_iterator begin() noexcept { return v_iterator(ptr_); }
  v_const_iterator begin() const noexcept { return cbegin(); }
  v_const_iterator cbegin() const noexcept { return v_const_iterator(ptr_); }
  v_iterator end() noexcept { return v_iterator(ptr_ + size_); }
  v_const_iterator end() const noexcept { return cend(); }
  v_const_iterator cend() const noexcept {
    return v_const_iterator(ptr_ + size_);
  }

  v_reverse_iterator rbegin() noexcept {
    return v_reverse_iterator(ptr_ + size_ - 1);
  }
  v_creverse_iterator rbegin() const noexcept { return crbegin(); }
  v_creverse_iterator crbegin() const noexcept {
    return v_creverse_iterator(ptr_ + size_ - 1);
  }
  v_reverse_iterator rend() noexcept { return v_reverse_iterator(ptr_ - 1); }
  v_creverse_iterator rend() const noexcept { return crend(); }
  v_creverse_iterator crend() const noexcept {
    return v_creverse_iterator(ptr_ - 1);
  }

  void assign(std::initializer_list<value_type> values) {
    assign(values.begin(), values.end());
  }

  void assign(size_t count, const_reference value);

  template <typename InputIterator>
  void assign(InputIterator start, InputIterator end);

  void reserve(size_t count) {
    if (count > max_size() || count < 0) {
      throw std::length_error("Invalid reserve space");
    } else if (count > capacity_) {
      pointer mem = al.allocate(count);
      al.move_chunk_left(ptr_, ptr_ + size_, mem);
      swap_buffers(mem);
      capacity_ = count;
    }
  }

  void shrink_to_fit() {
    if (size_ < capacity_) {
      pointer mem = al.allocate(size_);
      al.move_chunk_left(ptr_, ptr_ + size_, mem);
      swap_buffers(mem);
      capacity_ = size_;
    }
  }

  void clear() {
    al.destroy_multiple(ptr_, size_);
    size_ = 0;
  }

  void resize(size_t count) { forward_resize(count); }

  void resize(size_t count, const_reference value) {
    forward_resize(count, value);
  }

  void push_back(const_reference value) { 
    insert(end(), std::forward<const value_type>(value));
  }

  void push_back(move_reference value) { 
    insert(end(), std::forward<value_type>(value));
  }

  void pop_back() {
    size_--;
    al.destroy(ptr_ + size_);
  }

  v_iterator insert(v_const_iterator pos, const_reference value) {
    return forward_insert<const_reference>(pos, std::forward<const value_type>(value));
  }

  v_iterator insert(v_const_iterator pos, move_reference value) {
    return forward_insert<move_reference>(pos, std::forward<value_type>(value));
  }

  v_iterator erase(v_const_iterator pos) {
    pointer dest = ptr_ + (pos - begin());
    al.destroy(dest);
    al.move_chunk_left(dest + 1, end().base(), dest);
    size_--;
    return v_iterator(dest);
  }

  // Note: the arguments are the already created elements of the appropriate
  // container that should be inserted into this container. This also requires
  // value_type to be MoveConstructible
  template <typename... Args>
  v_iterator insert_many(v_const_iterator pos, Args&&... values);

  template <typename... Args>
  void insert_many_back(Args&&... values) {
    insert_many(end(), values...);
  }

  template <typename... Args>
  v_iterator emplace(v_const_iterator pos, Args&&... values) {
    return insert_many(pos, values...);
  }

  template <typename... Args>
  void emplace_back(Args&&... values) {
    insert_many(end(), values...);
  }

  void swap(vector& other) noexcept { *this = std::move(other); }

  reference operator[](size_t i) { return ptr_[i]; }
  value_type operator[](size_t i) const { return ptr_[i]; }

  bool operator==(const vector& other) const {
    if (size_ != other.size_) return false;
    for (size_t i = 0; i < size_; i++)
      if (ptr_[i] != other.ptr_[i]) return false;
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os, const vector& vec) {
    for (size_t i = 0; i < vec.size_; i++) os << vec.ptr_[i] << ' ';
    return os;
  }

 private:
  void swap_buffers(pointer temporary) {
    std::swap(temporary, ptr_);
    al.deallocate(temporary);
  }

  template <typename Ref>
  v_iterator forward_insert(v_const_iterator where, Ref value);

  template <typename... Args>
  void forward_resize(size_t count, Args&&... value);

  size_t size_;
  size_t capacity_;
  pointer ptr_;
  allocator<T> al;
};

template <typename T>
void vector<T>::assign(size_t count, const_reference value) {
  if (count > max_size() || count < 0) {
    throw std::length_error("Invalid assign count");
  }
  pointer mem = ptr_;
  al.destroy_multiple(ptr_, size_);
  if (count > capacity_) {
    mem = al.allocate(count);
    capacity_ = count;
    swap_buffers(mem);
  }
  al.construct_multiple(mem, count, value);
  size_ = count;
}

template <typename T>
template <typename InputIterator>
void vector<T>::assign(InputIterator start, InputIterator end) {
  size_t count = std::distance(start, end);
  if (count > max_size() || count < 0) {
    throw std::length_error("Distance between start and end iterators is negative or too big");
  }
  pointer mem = ptr_;
  al.destroy_multiple(mem, size_);
  if (count > capacity_) {
    mem = al.allocate(count);
    capacity_ = count;
    swap_buffers(mem);
  }
  al.construct(mem, start, end);
  size_ = count;
}

template <typename T>
template <typename Ref>
typename vector<T>::v_iterator vector<T>::forward_insert(v_const_iterator pos,
                                                         Ref value) {
  size_t delta = pos - begin();
  if (size_ >= capacity_) {
    pointer mem = al.allocate(std::max(capacity_ * kCapMul, size_ + 1));
    capacity_ = std::max(capacity_ * kCapMul, size_ + 1);
    al.move_chunk_left(ptr_, ptr_ + delta, mem);
    al.move_chunk_left(ptr_ + delta, ptr_ + size_, mem + delta + 1);
    swap_buffers(mem);
  } else {
    al.move_chunk_right(ptr_ + delta, end().base(), ptr_ + delta + 1);
  }
  al.construct(ptr_ + delta, std::forward<Ref>(value));
  size_++;
  return v_iterator(ptr_ + delta);
}

template <typename T>
template <typename... Args>
void vector<T>::forward_resize(size_t count, Args&&... value) {
  if (count > max_size() || count < 0) {
    throw std::length_error("Invalid resize space");
  }
  if (size_ == count)
    return;
  else if (size_ > count) {
    al.destroy_multiple(ptr_ + count, size_ - count);
  } else {
    if (count > capacity_) {
      pointer mem = al.allocate(count);
      capacity_ = count;
      al.move_chunk_left(ptr_, ptr_ + size_, mem);
      swap_buffers(mem);
    }
    al.construct_multiple(ptr_ + size_, count - size_, std::forward<Args>(value)...);
  }
  size_ = count;
}

template <typename T>
template <typename... Args>
typename vector<T>::v_iterator vector<T>::insert_many(v_const_iterator pos,
                                                      Args&&... values) {
  size_t delta = pos.base() - ptr_;
  size_t count = sizeof...(Args);
  pointer mem = ptr_;
  if (size_ + count >= capacity_) {
    mem = al.allocate(std::max(size_ + count, capacity_ * kCapMul));
    capacity_ = std::max(size_ + count, capacity_ * kCapMul);
    al.move_chunk_left(ptr_, ptr_ + delta, mem);
    al.move_chunk_right(ptr_ + delta, ptr_ + size_, mem + delta + count);
    swap_buffers(mem);
    mem = ptr_;
  } else {
    al.move_chunk_right(ptr_ + delta, ptr_ + size_, mem + delta + count);
  }
  for (auto value : {values...}) {
    al.construct(mem++ + delta, value);
  }
  size_ += count;
  return v_iterator(ptr_ + delta);
}
}  // namespace s21

#endif  // SRC_CONTAINERS_S21_VECTOR_H_
