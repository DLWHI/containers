#ifndef DLWHI_INCLUDE_VECTOR_H_
#define DLWHI_INCLUDE_VECTOR_H_
#include <cstdint>
#include <initializer_list>
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "iterator.h"

namespace dlwhi {
typedef int64_t size_t;

template <typename T>
class alloc_wrap {
 public:
  typedef T allocator_type;
  typedef typename allocator_type::value_type* pointer;

  alloc_wrap(const allocator_type& al) : al_(al) { };

  allocator_type& allocator() { return al_;}
  const allocator_type& allocator() const { return al_;}

  pointer allocate(dlwhi::size_t count) { return al_.allocate(count);}
  void deallocate(pointer ptr_, dlwhi::size_t cap) { al_.deallocate(ptr_, cap);}

  void move_chunk_right(pointer left, pointer right, pointer where) {
    where += right - left - 1;
    for (pointer i = right - 1; i != left - 1; i--) {
      new (where--) T(move_if_possible(*i));
      i->~T();
    }
  }

  void move_chunk_left(pointer left, pointer right, pointer where) {
    for (pointer i = left; i != right; i++) {
      new (where++) T(move_if_possible(*i));
      i->~T();
    }
  }

  template <typename... Args>
  void construct(pointer where, Args&&... args) {
    al_.construct(where, std::forward<Args>(args)...);
  }

  void destroy(pointer where) {
    al_.destroy(where);
  }


  void destroy_multiple(pointer where, dlwhi::size_t count) {
    for (dlwhi::size_t i = 0; i < count; i++, where++) al_.destroy(where);
  }

 private:
  typename std::conditional<std::is_move_constructible<T>::value, T&&,
                            const T&>::type
  move_if_possible(T& what) {
    return std::move(what);
  }

  allocator_type al_;
};

// Requires T to be at least CopyConstructible and CopyASSignable
template <typename T, class Allocator = std::allocator<T>>
class vector {
 public:
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T&& move_reference;

  typedef Allocator allocator_type;

  typedef dlwhi::iterator<T, vector> iterator;
  typedef dlwhi::iterator<const T, vector> const_iterator;
  typedef dlwhi::reverse_iterator<T, vector> reverse_iterator;
  typedef dlwhi::reverse_iterator<const T, vector> const_reverse_iterator;

  static constexpr dlwhi::size_t kCapMul = 2;

  constexpr vector() noexcept : size_(0), capacity_(0), ptr_(nullptr), al_(Allocator()){};

  constexpr explicit vector(const Allocator& alloc) noexcept 
      : size_(0), capacity_(0), ptr_(nullptr), al_(alloc){};

  constexpr explicit vector(dlwhi::size_t size, const Allocator& alloc = Allocator()) 
      : size_(size), capacity_(size_), al_(alloc) {
    ptr_ = al_.allocate(capacity_);
    construct_multiple(ptr_, size_);
  };

  constexpr explicit vector(dlwhi::size_t size, const_reference value, const Allocator& alloc = Allocator())
      : size_(size), capacity_(size_), al_(alloc) {
    ptr_ = al_.allocate(capacity_);
    construct_multiple(ptr_, size_, value);
  };

  template <typename InputIterator>
  constexpr vector(const InputIterator& start, const InputIterator& end, const Allocator& alloc = Allocator())
      : size_(std::distance(start, end)), capacity_(size_), al_(alloc) {
    ptr_ = al_.allocate(capacity_);
    construct_multiple(ptr_, start, end);
  }

  constexpr vector(std::initializer_list<value_type> values, const Allocator& alloc = Allocator())
      : vector(values.begin(), values.end(), alloc){};

  constexpr vector(const vector& other) : vector(other.begin(), other.end(), other.al_){};

  constexpr vector(vector&& other) noexcept : vector() { *this = std::move(other); };

  constexpr vector(const vector& other, const Allocator& alloc) 
      : vector(other.begin(), other.end(), alloc){};

  constexpr vector(vector&& other, const Allocator& alloc) noexcept 
    : size_(0), capacity_(0), ptr_(nullptr), al_(alloc) { 
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
    std::swap(ptr_, other.ptr_);
  };

  constexpr vector& operator=(const vector& other) {
    vector cpy(other);
    *this = std::move(cpy);
    return *this;
  }

  constexpr vector& operator=(vector&& other) noexcept {
    std::swap(capacity_, other.capacity_);
    std::swap(size_, other.size_);
    std::swap(ptr_, other.ptr_);
    al_ = std::move(other.al_);
    return *this;
  }

  virtual ~vector() {
    destroy_multiple(ptr_, size_);
    al_.deallocate(ptr_, capacity_);
  }

  constexpr allocator_type get_allocator() { 
    return allocator_type(al_);
  }

  constexpr reference at(dlwhi::size_t pos) {
    return (pos < size_) ? ptr_[pos] : throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr const_reference at(dlwhi::size_t pos) const {
    return (pos < size_) ? ptr_[pos] : throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr reference front() noexcept { return ptr_[0]; }
  constexpr reference back() noexcept { return ptr_[size_ - 1]; }
  constexpr pointer data() noexcept { return ptr_; }

  constexpr const_reference front() const noexcept { return ptr_[0]; }
  constexpr const_reference back() const noexcept { return ptr_[size_ - 1]; }
  constexpr const_pointer data() const noexcept { return ptr_; }

  constexpr bool empty() const noexcept { return !size_; }
  constexpr dlwhi::size_t size() const noexcept { return size_; }
  constexpr dlwhi::size_t max_size() const noexcept {
    return std::allocator_traits<Allocator>::max_size(al_);
  }
  constexpr dlwhi::size_t capacity() const noexcept { return capacity_; }

  constexpr iterator begin() noexcept { return iterator(ptr_); }
  constexpr const_iterator begin() const noexcept { return cbegin(); }
  constexpr const_iterator cbegin() const noexcept { return const_iterator(ptr_); }
  constexpr iterator end() noexcept { return iterator(ptr_ + size_); }
  constexpr const_iterator end() const noexcept { return cend(); }
  constexpr const_iterator cend() const noexcept {
    return const_iterator(ptr_ + size_);
  }

  constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(ptr_ + size_ - 1);
  }
  constexpr const_reverse_iterator rbegin() const noexcept { return crbegin(); }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(ptr_ + size_ - 1);
  }
  constexpr reverse_iterator rend() noexcept { return reverse_iterator(ptr_ - 1); }
  constexpr const_reverse_iterator rend() const noexcept { return crend(); }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(ptr_ - 1);
  }

  constexpr void assign(std::initializer_list<value_type> values) {
    assign(values.begin(), values.end());
  }

  constexpr void assign(dlwhi::size_t count, const_reference value);

  template <typename InputIterator>
  constexpr void assign(InputIterator start, InputIterator end);

  constexpr void reserve(dlwhi::size_t count) {
    if (count > max_size() || count < 0) {
      throw std::length_error("Invalid reserve space");
    } else if (count > capacity_) {
      pointer mem = al_.allocate(count);
      move_chunk_left(ptr_, ptr_ + size_, mem);
      swap_buffers(mem);
      capacity_ = count;
    }
  }

  void shrink_to_fit() {
    if (size_ < capacity_) {
      pointer mem = al_.allocate(size_);
      // al_.move_chunk_left(ptr_, ptr_ + size_, mem);
      swap_buffers(mem);
      capacity_ = size_;
    }
  }

  void clear() {
    destroy_multiple(ptr_, size_);
    size_ = 0;
  }

  void resize(dlwhi::size_t count) { forward_resize(count); }

  void resize(dlwhi::size_t count, const_reference value) {
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
    std::destroy_at(ptr_ + size_);
  }

  iterator insert(const_iterator pos, const_reference value) {
    return forward_insert<const_reference>(pos, std::forward<const value_type>(value));
  }

  iterator insert(const_iterator pos, move_reference value) {
    return forward_insert<move_reference>(pos, std::forward<value_type>(value));
  }

  iterator erase(const_iterator pos) {
    pointer dest = ptr_ + (pos - begin());
    std::destroy_at(dest);
    // al_.move_chunk_left(dest + 1, end().base(), dest);
    size_--;
    return iterator(dest);
  }

  // Note: the arguments are the already created elements of the appropriate
  // container that should be inserted into this container. This also requires
  // value_type to be MoveConstructible
  template <typename... Args>
  iterator insert_many(const_iterator pos, Args&&... values);

  template <typename... Args>
  void insert_many_back(Args&&... values) {
    insert_many(end(), values...);
  }

  template <typename... Args>
  iterator emplace(const_iterator pos, Args&&... values) {
    return insert_many(pos, values...);
  }

  template <typename... Args>
  void emplace_back(Args&&... values) {
    insert_many(end(), values...);
  }

  void swap(vector& other) noexcept { *this = std::move(other); }

  reference operator[](dlwhi::size_t i) { return ptr_[i]; }
  value_type operator[](dlwhi::size_t i) const { return ptr_[i]; }

  constexpr bool operator==(const vector& other) const {
    if (size_ != other.size_) return false;
    for (dlwhi::size_t i = 0; i < size_; i++)
      if (ptr_[i] != other.ptr_[i]) return false;
    return true;
  }

  constexpr bool operator!=(const vector& other) const {
    return !(*this == other);
  }

  friend std::ostream& operator<<(std::ostream& os, const vector& vec) {
    for (dlwhi::size_t i = 0; i < vec.size_ - 1; i++) os << vec.ptr_[i] << ' ';
    os << vec.ptr_[vec.size_ - 1];
    return os;
  }

 private:
  template <typename InputIterator>
  constexpr void construct_multiple(pointer where, InputIterator left, InputIterator right) {
    for (; left != right; left++, where++) 
      std::construct_at(where, *left);
  }

  template <typename... Args>
  constexpr void construct_multiple(pointer where, dlwhi::size_t count, Args&&... args) {
    for (dlwhi::size_t i = 0; i < count; i++, where++) 
      std::construct_at(where, std::forward<Args>(args)...);
  }

  constexpr void destroy_multiple(pointer start, dlwhi::size_t count) {
    for (dlwhi::size_t i = 0; i < count; i++, start++) std::destroy_at(start);
  }

  constexpr void move_chunk_left(pointer left, pointer right, pointer where) {
    for (; left != right; left++, where++) {
      std::construct_at(where, std::forward<value_type>(*left));
      left->~T();
    }
  }

  void swap_buffers(pointer temporary) {
    std::swap(temporary, ptr_);
    al_.deallocate(temporary, capacity_);
  }

  template <typename Ref>
  iterator forward_insert(const_iterator where, Ref value);

  template <typename... Args>
  void forward_resize(dlwhi::size_t count, Args&&... value);

  dlwhi::size_t size_;
  dlwhi::size_t capacity_;
  pointer ptr_;
  allocator_type al_;
};

template <typename T, class Allocator>
constexpr void vector<T, Allocator>::assign(dlwhi::size_t count, const_reference value) {
  if (count > max_size() || count < 0) {
    throw std::length_error("Invalid assign count");
  }
  pointer mem = ptr_;
  destroy_multiple(ptr_, size_);
  if (count > capacity_) {
    mem = al_.allocate(count);
    swap_buffers(mem);
    capacity_ = count;
  }
  construct_multiple(mem, count, value);
  size_ = count;
}

template <typename T, class Allocator>
template <typename InputIterator>
constexpr void vector<T, Allocator>::assign(InputIterator start, InputIterator end) {
  dlwhi::size_t count = std::distance(start, end);
  if (count > max_size() || count < 0) {
    throw std::length_error("Distance between start and end iterators is negative or too big");
  }
  pointer mem = ptr_;
  destroy_multiple(mem, size_);
  if (count > capacity_) {
    mem = al_.allocate(count);
    swap_buffers(mem);
    capacity_ = count;
  }
  construct_multiple(mem, start, end);
  size_ = count;
}

template <typename T, class Allocator>
template <typename Ref>
typename vector<T, Allocator>::iterator 
vector<T, Allocator>::forward_insert(const_iterator pos,
                                      Ref value) {
  dlwhi::size_t delta = pos - begin();
  if (size_ >= capacity_) {
    // pointer mem = al_.allocate(std::max(capacity_ * kCapMul, size_ + 1));
    capacity_ = std::max(capacity_ * kCapMul, size_ + 1);
    // al_.move_chunk_left(ptr_, ptr_ + delta, mem);
    // al_.move_chunk_left(ptr_ + delta, ptr_ + size_, mem + delta + 1);
    // swap_buffers(mem);
  } else {
    // al_.move_chunk_right(ptr_ + delta, end().base(), ptr_ + delta + 1);
  }
  // al_.construct(ptr_ + delta, std::forward<Ref>(value));
  size_++;
  return iterator(ptr_ + delta);
}

template <typename T, class Allocator>
template <typename... Args>
void vector<T, Allocator>::forward_resize(dlwhi::size_t count, Args&&... value) {
  if (count > max_size() || count < 0) {
    throw std::length_error("Invalid resize space");
  }
  if (size_ == count)
    return;
  else if (size_ > count) {
    al_.destroy_multiple(ptr_ + count, size_ - count);
  } else {
    if (count > capacity_) {
      pointer mem = al_.allocate(count);
      capacity_ = count;
      al_.move_chunk_left(ptr_, ptr_ + size_, mem);
      swap_buffers(mem);
    }
    al_.construct_multiple(ptr_ + size_, count - size_, std::forward<Args>(value)...);
  }
  size_ = count;
}

template <typename T, class Allocator>
template <typename... Args>
typename vector<T, Allocator>::iterator 
vector<T, Allocator>::insert_many(const_iterator pos,
                                  Args&&... values) {
  dlwhi::size_t delta = pos.base() - ptr_;
  dlwhi::size_t count = sizeof...(Args);
  pointer mem = ptr_;
  if (size_ + count >= capacity_) {
    mem = al_.allocate(std::max(size_ + count, capacity_ * kCapMul));
    capacity_ = std::max(size_ + count, capacity_ * kCapMul);
    al_.move_chunk_left(ptr_, ptr_ + delta, mem);
    al_.move_chunk_right(ptr_ + delta, ptr_ + size_, mem + delta + count);
    swap_buffers(mem);
    mem = ptr_;
  } else {
    al_.move_chunk_right(ptr_ + delta, ptr_ + size_, mem + delta + count);
  }
  for (auto value : {values...}) {
    al_.construct(mem++ + delta, value);
  }
  size_ += count;
  return iterator(ptr_ + delta);
}
}  // namespace dlwhi

#endif  // DLWHI_INCLUDE_VECTOR_H_
