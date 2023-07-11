#ifndef SRC_CONTAINERS_S21_ITERATOR_H_
#define SRC_CONTAINERS_S21_ITERATOR_H_
#include <algorithm>
#include <cstdint>

namespace dlwhi {
// T - pointer type
// Container - is not used inside of class, but allows different containers
//             with same template type produce different iterators
template <typename T, typename Container>
class iterator {
 public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef int64_t difference_type;
  typedef T* pointer;
  typedef T& reference;

  constexpr iterator() noexcept : ptr_(nullptr){};
  explicit iterator(T* data) noexcept : ptr_(data){};
  iterator(const iterator& other) noexcept : ptr_(other.ptr_){};
  iterator(iterator&& other) noexcept : ptr_(nullptr) {
    std::swap(ptr_, other.ptr_);
  }
  iterator& operator=(const iterator& other) noexcept {
    ptr_ = other.ptr_;
    return *this;
  }
  iterator& operator=(iterator&& other) noexcept {
    ptr_ = other.ptr_;
    return *this;
  }
  virtual ~iterator() = default;

  T* base() const noexcept { return ptr_; }

  T& operator*() const noexcept { return *ptr_; }
  T* operator->() const noexcept { return ptr_; }

  bool operator==(const iterator& other) const noexcept {
    return ptr_ == other.ptr_;
  }

  bool operator!=(const iterator& other) const noexcept {
    return ptr_ != other.ptr_;
  }

  bool operator>(const iterator& other) const noexcept {
    return (ptr_ - other.ptr_) > 0;
  }

  bool operator<(const iterator& other) const noexcept {
    return (ptr_ - other.ptr_) < 0;
  }

  bool operator>=(const iterator& other) const noexcept {
    return (ptr_ - other.ptr_) >= 0;
  }

  bool operator<=(const iterator& other) const noexcept {
    return (ptr_ - other.ptr_) <= 0;
  }

  iterator operator+(difference_type delta) const noexcept {
    return iterator(ptr_ + delta);
  }

  iterator operator-(difference_type delta) const noexcept {
    return iterator(ptr_ - delta);
  }

  difference_type operator-(const iterator& other) const noexcept {
    return ptr_ - other.ptr_;
  }

  iterator operator++(int) noexcept { return iterator(ptr_++); }
  iterator operator--(int) noexcept { return iterator(ptr_--); }

  iterator& operator++() noexcept {
    ++ptr_;
    return *this;
  }

  iterator& operator--() noexcept {
    --ptr_;
    return *this;
  }

  operator iterator<const T, Container>() const noexcept {
    return iterator<const T, Container>(const_cast<const T*>(ptr_));
  }

 protected:
  T* ptr_;
};

template <typename T, typename Container>
class reverse_iterator {
 public:
  typedef std::random_access_iterator_tag iterator_category;
  typedef T value_type;
  typedef int64_t difference_type;
  typedef T* pointer;
  typedef T& reference;

  constexpr reverse_iterator() noexcept : ptr_(nullptr){};
  explicit reverse_iterator(T* data) noexcept : ptr_(data){};
  reverse_iterator(const reverse_iterator& other) noexcept : ptr_(other.ptr_){};
  reverse_iterator(reverse_iterator&& other) noexcept : ptr_(nullptr) {
    std::swap(ptr_, other.ptr_);
  }
  reverse_iterator& operator=(const reverse_iterator& other) noexcept {
    ptr_ = other.ptr_;
    return *this;
  }
  reverse_iterator& operator=(reverse_iterator&& other) noexcept {
    ptr_ = other.ptr_;
    return *this;
  }
  virtual ~reverse_iterator() = default;

  T* base() const noexcept { return ptr_; }

  T& operator*() const noexcept { return *ptr_; }
  T* operator->() const noexcept { return *ptr_; }

  bool operator==(const reverse_iterator& other) const noexcept {
    return ptr_ == other.ptr_;
  }

  bool operator!=(const reverse_iterator& other) const noexcept {
    return ptr_ != other.ptr_;
  }

  bool operator>(const reverse_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) < 0;
  }

  bool operator<(const reverse_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) > 0;
  }

  bool operator>=(const reverse_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) <= 0;
  }

  bool operator<=(const reverse_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) >= 0;
  }

  reverse_iterator operator+(difference_type delta) const noexcept {
    return reverse_iterator(ptr_ - delta);
  }

  reverse_iterator operator-(difference_type delta) const noexcept {
    return reverse_iterator(ptr_ + delta);
  }

  difference_type operator-(const reverse_iterator& other) const noexcept {
    return other.ptr_ - ptr_;
  }

  reverse_iterator operator++(int) noexcept { return reverse_iterator(ptr_--); }
  reverse_iterator operator--(int) noexcept { return reverse_iterator(ptr_++); }

  reverse_iterator& operator++() noexcept {
    --ptr_;
    return *this;
  }

  reverse_iterator& operator--() noexcept {
    ++ptr_;
    return *this;
  }

  operator reverse_iterator<const T, Container>() const noexcept {
    return reverse_iterator<const T, Container>(const_cast<const T*>(ptr_));
  }

 protected:
  T* ptr_;
};
}  // namespace s21
#endif  // SRC_CONTAINERS_S21_ITERATOR_H_
