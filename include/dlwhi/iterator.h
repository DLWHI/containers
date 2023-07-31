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
  constexpr explicit iterator(T* data) noexcept : ptr_(data){};
  constexpr virtual ~iterator() = default;

  constexpr T* base() const noexcept { return ptr_; }

  constexpr T& operator*() const noexcept { return *ptr_; }
  constexpr T* operator->() const noexcept { return ptr_; }

  constexpr bool operator==(const iterator& other) const noexcept {
    return ptr_ == other.ptr_;
  }

  constexpr bool operator!=(const iterator& other) const noexcept {
    return ptr_ != other.ptr_;
  }

  constexpr bool operator>(const iterator& other) const noexcept {
    return (ptr_ - other.ptr_) > 0;
  }

  constexpr bool operator<(const iterator& other) const noexcept {
    return (ptr_ - other.ptr_) < 0;
  }

  constexpr bool operator>=(const iterator& other) const noexcept {
    return (ptr_ - other.ptr_) >= 0;
  }

  constexpr bool operator<=(const iterator& other) const noexcept {
    return (ptr_ - other.ptr_) <= 0;
  }

  constexpr iterator operator+(difference_type delta) const noexcept {
    return iterator(ptr_ + delta);
  }

  constexpr iterator operator-(difference_type delta) const noexcept {
    return iterator(ptr_ - delta);
  }

  constexpr difference_type operator-(const iterator& other) const noexcept {
    return ptr_ - other.ptr_;
  }

  constexpr iterator operator++(int) noexcept { return iterator(ptr_++); }
  constexpr iterator operator--(int) noexcept { return iterator(ptr_--); }

  constexpr iterator& operator++() noexcept {
    ++ptr_;
    return *this;
  }

  constexpr iterator& operator--() noexcept {
    --ptr_;
    return *this;
  }

  constexpr operator iterator<const T, Container>() const noexcept {
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
  constexpr explicit reverse_iterator(T* data) noexcept : ptr_(data){};
  constexpr virtual ~reverse_iterator() = default;

  constexpr T* base() const noexcept { return ptr_; }

  constexpr T& operator*() const noexcept { return *ptr_; }
  constexpr T* operator->() const noexcept { return *ptr_; }

  constexpr bool operator==(const reverse_iterator& other) const noexcept {
    return ptr_ == other.ptr_;
  }

  constexpr bool operator!=(const reverse_iterator& other) const noexcept {
    return ptr_ != other.ptr_;
  }

  constexpr bool operator>(const reverse_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) < 0;
  }

  constexpr bool operator<(const reverse_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) > 0;
  }

  constexpr bool operator>=(const reverse_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) <= 0;
  }

  constexpr bool operator<=(const reverse_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) >= 0;
  }

  constexpr reverse_iterator operator+(difference_type delta) const noexcept {
    return reverse_iterator(ptr_ - delta);
  }

  constexpr reverse_iterator operator-(difference_type delta) const noexcept {
    return reverse_iterator(ptr_ + delta);
  }

  constexpr difference_type operator-(const reverse_iterator& other) const noexcept {
    return other.ptr_ - ptr_;
  }

  constexpr reverse_iterator operator++(int) noexcept { return reverse_iterator(ptr_--); }
  constexpr reverse_iterator operator--(int) noexcept { return reverse_iterator(ptr_++); }

  constexpr reverse_iterator& operator++() noexcept {
    --ptr_;
    return *this;
  }

  constexpr reverse_iterator& operator--() noexcept {
    ++ptr_;
    return *this;
  }

  constexpr operator reverse_iterator<const T, Container>() const noexcept {
    return reverse_iterator<const T, Container>(const_cast<const T*>(ptr_));
  }

 protected:
  T* ptr_;
};
}  // namespace s21
#endif  // SRC_CONTAINERS_S21_ITERATOR_H_
