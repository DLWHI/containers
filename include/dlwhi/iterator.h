#ifndef DLWHI_CONTAINERS_ITERATOR_H_
#define DLWHI_CONTAINERS_ITERATOR_H_
#include <iterator>
#include <cstdint>

namespace dlwhi {

using diff_t = int64_t;

// T - pointer type
// Container - is not used inside of class, but allows different containers
//             with same template type produce different iterators
template <typename T, typename Container>
class iterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = T;
  using pointer = T*;
  using reference = T&;

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

  constexpr iterator operator+(diff_t delta) const noexcept {
    return iterator(ptr_ + delta);
  }

  constexpr iterator operator-(diff_t delta) const noexcept {
    return iterator(ptr_ - delta);
  }

  constexpr diff_t operator-(const iterator& other) const noexcept {
    return ptr_ - other.ptr_;
  }

  constexpr iterator& operator+=(diff_t delta) noexcept {
    ptr_ += delta;
    return *this;
  }

  constexpr iterator& operator-=(diff_t delta) noexcept {
    ptr_ -= delta;
    return *this;
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
}  // namespace s21
#endif  // DLWHI_CONTAINERS_ITERATOR_H_
