#ifndef DLWHI_CONTAINERS_VECTOR_H_
#define DLWHI_CONTAINERS_VECTOR_H_
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "iterator.h"
#include "reverse_iterator.h"

namespace dlwhi {

using size_t = int64_t;

// Requires T to be at least CopyConstructible
template <typename T, class Allocator = std::allocator<T>>
class vector {
 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using move_reference = T&&;

  using allocator_type = Allocator;
  using al_traits = std::allocator_traits<allocator_type>;

  using iterator = dlwhi::iterator<T, vector>;
  using const_iterator = dlwhi::iterator<const T, vector>;
  using reverse_iterator = dlwhi::reverse_iterator<iterator>;
  using const_reverse_iterator = dlwhi::reverse_iterator<const_iterator>;

  static constexpr dlwhi::size_t kCapMul = 2;

  constexpr vector() noexcept(noexcept(Allocator())) : al_(Allocator()), ptr_(nullptr), size_(0), capacity_(0){};

  constexpr explicit vector(const Allocator& alloc) noexcept 
      : al_(alloc), size_(0), capacity_(0), ptr_(nullptr){};

  constexpr explicit vector(dlwhi::size_t size, const Allocator& alloc = Allocator()) 
      : al_(alloc), size_(size), capacity_(size_), ptr_(al_.allocate(size)) {
    construct_multiple(ptr_, size_);
  };

  constexpr explicit vector(dlwhi::size_t size, const_reference value, const Allocator& alloc = Allocator())
      : al_(alloc), size_(size), capacity_(size_), ptr_(al_.allocate(size)) {
    construct_multiple(ptr_, size_, value);
  };

  template <typename InputIterator>
  constexpr vector(const InputIterator& start, const InputIterator& end, const Allocator& alloc = Allocator())
      : al_(alloc), size_(std::distance(start, end)), capacity_(size_), ptr_(al_.allocate(size_)) {
    construct_multiple(ptr_, start, end);
  }

  constexpr vector(std::initializer_list<value_type> values, const Allocator& alloc = Allocator())
      : vector(values.begin(), values.end(), alloc){};

  constexpr vector(const vector& other) : vector(other.begin(), other.end(), other.al_){};

  constexpr vector(vector&& other) noexcept : vector() { *this = std::move(other);}

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

  constexpr virtual ~vector() {
    destroy_multiple(ptr_, size_);
    al_.deallocate(ptr_, capacity_);
  }

  constexpr allocator_type get_allocator() { 
    return allocator_type(al_);
  }

  constexpr reference at(dlwhi::size_t pos) {
    return (0 <= pos && pos < size_) ? ptr_[pos] : throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr const_reference at(dlwhi::size_t pos) const {
    return (0 <= pos && pos < size_) ? ptr_[pos] : throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr reference front() noexcept { return ptr_[0]; }
  constexpr reference back() noexcept { return ptr_[size_ - 1]; }
  constexpr pointer data() noexcept { return ptr_; }

  constexpr const_reference front() const noexcept { return ptr_[0]; }
  constexpr const_reference back() const noexcept { return ptr_[size_ - 1]; }
  constexpr const_pointer data() const noexcept { return ptr_; }

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

  constexpr bool empty() const noexcept { return !size_; }
  constexpr dlwhi::size_t size() const noexcept { return size_; }
  constexpr dlwhi::size_t capacity() const noexcept { return capacity_; }
  constexpr dlwhi::size_t max_size() const noexcept {
    return al_traits::max_size(al_);
  }

  constexpr vector& assign(std::initializer_list<value_type> values) {
    return assign(values.begin(), values.end());
  }

  constexpr vector& assign(dlwhi::size_t count, const_reference value);

  template <typename InputIterator>
  constexpr vector& assign(InputIterator start, InputIterator end);

  constexpr vector& reserve(dlwhi::size_t count) {
    if (count > max_size() || count < 0) {
      throw std::length_error("Invalid reserve space");
    } else if (count > capacity_) {
      pointer mem = al_.allocate(count);
      move_chunk_left(ptr_, ptr_ + size_, mem);
      swap_buffers(mem);
      capacity_ = count;
    }
    return *this;
  }

  constexpr vector& shrink_to_fit() {
    if (size_ < capacity_) {
      pointer mem = al_.allocate(size_);
      move_chunk_left(ptr_, ptr_ + size_, mem);
      swap_buffers(mem);
      capacity_ = size_;
    }
    return *this;
  }

  constexpr vector& clear() {
    destroy_multiple(ptr_, size_);
    size_ = 0;
    return *this;
  }

  constexpr vector& resize(dlwhi::size_t count) { return resize(count, value_type()); }

  constexpr vector& resize(dlwhi::size_t count, const_reference value);

  constexpr vector& push_back(const_reference value) { 
    place_at(end(), std::forward<const value_type>(value));
    return *this;
  }

  constexpr vector& push_back(move_reference value) { 
    place_at(end(), std::forward<value_type>(value));
    return *this;
  }

  constexpr vector& pop_back() {
    size_--;
    al_traits::destroy(al_, ptr_ + size_);
    return *this;
  }

  constexpr iterator insert(const_iterator pos, const_reference value) {
    return place_at<const_reference>(pos, std::forward<const value_type>(value));
  }

  constexpr iterator insert(const_iterator pos, move_reference value) {
    return place_at<move_reference>(pos, std::forward<value_type>(value));
  }

  constexpr iterator erase(const_iterator pos) {
    pointer dest = ptr_ + (pos - begin());
    al_traits::destroy(al_, dest);
    move_chunk_left(dest + 1, end().base(), dest);
    size_--;
    return iterator(dest);
  }

  template <typename... Args>
  constexpr iterator emplace(const_iterator pos, Args&&... values) {
    return place_at<Args...>(pos, std::forward<Args>(values)...);
  }

  template <typename... Args>
  constexpr vector& emplace_back(Args&&... values) {
    place_at(end(), std::forward<Args>(values)...);
    return *this;
  }

  constexpr reference operator[](dlwhi::size_t i) { return ptr_[i]; }
  constexpr value_type operator[](dlwhi::size_t i) const { return ptr_[i]; }

  constexpr vector& swap(vector& other) noexcept(al_traits::propagate_on_container_swap::value
                                              || al_traits::is_always_equal::value) {
    if constexpr (al_traits::propagate_on_container_swap::value)
      std::swap(al_, other.al_);
    std::swap(ptr_, other.ptr_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
    return *this;
  }

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
      al_traits::construct(al_, where, *left);
  }

  template <typename... Args>
  constexpr void construct_multiple(pointer where, dlwhi::size_t count, Args&&... args) {
    for (dlwhi::size_t i = 0; i < count; ++i, ++where) 
      al_traits::construct(al_, where, std::forward<Args>(args)...);
  }

  constexpr void destroy_multiple(pointer start, dlwhi::size_t count) {
    for (dlwhi::size_t i = 0; i < count; ++i, ++start) al_traits::destroy(al_, start);
  }

  constexpr void move_chunk_left(pointer left, pointer right, pointer where) {
    while (left != right) {
      if constexpr (std::is_move_constructible<T>::value)
        al_traits::construct(al_, where++, std::move(*left));
      else
        al_traits::construct(al_, where++, *left);
      al_traits::destroy(al_, left++);
    }
  }

  constexpr void move_chunk_right(pointer left, pointer right, pointer where) {
    where += right - left;
    while (left != right) {
      if constexpr (std::is_move_constructible<T>::value)
        al_traits::construct(al_, --where, std::move(*--right));
      else
        al_traits::construct(al_, --where, *--right);
      al_traits::destroy(al_, right);
    }
  }

  constexpr void swap_buffers(pointer temporary) {
    std::swap(temporary, ptr_);
    al_.deallocate(temporary, capacity_);
  }

  template <typename... Args>
  constexpr iterator place_at(const_iterator pos, Args&&... value);

  allocator_type al_;
  dlwhi::size_t size_;
  dlwhi::size_t capacity_;
  pointer ptr_;
};

template <typename T, class Allocator>
constexpr vector<T, Allocator>&
vector<T, Allocator>::assign(dlwhi::size_t count, const_reference value) {
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
  return *this;
}

template <typename T, class Allocator>
template <typename InputIterator>
constexpr vector<T, Allocator>&
vector<T, Allocator>::assign(InputIterator start, InputIterator end) {
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
  return *this;
}

template <typename T, class Allocator>
constexpr vector<T, Allocator>&
vector<T, Allocator>::resize(dlwhi::size_t count, const_reference value) {
  if (count > max_size() || count < 0) {
    throw std::length_error("Invalid resize space");
  }
  if (size_ == count)
    return *this;
  else if (size_ > count) {
    destroy_multiple(ptr_ + count, size_ - count);
  } else {
    if (count > capacity_) {
      pointer mem = al_.allocate(count);
      move_chunk_left(ptr_, ptr_ + size_, mem);
      swap_buffers(mem);
      capacity_ = count;
    }
    construct_multiple(ptr_ + size_, count - size_, value);
  }
  size_ = count;
  return *this;
}

template <typename T, class Allocator>
template <typename... Args>
constexpr typename vector<T, Allocator>::iterator 
vector<T, Allocator>::place_at(const_iterator pos,
                               Args&&... value) {
  dlwhi::size_t delta = pos - begin();
  if (size_ >= capacity_) {
    pointer mem = al_.allocate(std::max(capacity_ * kCapMul, size_ + 1));
    al_traits::construct(al_, mem + delta, std::forward<Args>(value)...);
    move_chunk_left(ptr_, ptr_ + delta, mem);
    move_chunk_left(ptr_ + delta, ptr_ + size_, mem + delta + 1);
    swap_buffers(mem);
    capacity_ = std::max(capacity_ * kCapMul, size_ + 1);
  } else {
    move_chunk_right(ptr_ + delta, ptr_ + size_, ptr_ + delta + 1);
    al_traits::construct(al_, ptr_ + delta, std::forward<Args>(value)...);
  }
  size_++;
  return iterator(ptr_ + delta);
}
}  // namespace dlwhi
#endif  // DLWHI_CONTAINERS_VECTOR_H_
