#ifndef SP_CONTAINERS_POINTER_ITERATOR_H_
#define SP_CONTAINERS_POINTER_ITERATOR_H_
#include <iterator>
#include <cstdint>

namespace sp {

using diff_t = int64_t;

// T - pointer type
// Container - is not used inside of class, but allows different containers
//             with same template type produce different iterators
template <typename T, typename Container>
class pointer_iterator {
 public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type = T;
  using pointer = T*;
  using reference = T&;
  using diff_t = int64_t;
  using difference_type = diff_t;

  constexpr pointer_iterator() noexcept : ptr_(nullptr){};
  constexpr explicit pointer_iterator(T* data) noexcept : ptr_(data){};
  constexpr virtual ~pointer_iterator() = default;

  constexpr T* data() const noexcept { return ptr_; }

  constexpr T& operator*() const noexcept { return *ptr_; }
  constexpr T* operator->() const noexcept { return ptr_; }

  constexpr bool operator==(const pointer_iterator& other) const noexcept {
    return ptr_ == other.ptr_;
  }

  constexpr bool operator!=(const pointer_iterator& other) const noexcept {
    return ptr_ != other.ptr_;
  }

  constexpr bool operator>(const pointer_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) > 0;
  }

  constexpr bool operator<(const pointer_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) < 0;
  }

  constexpr bool operator>=(const pointer_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) >= 0;
  }

  constexpr bool operator<=(const pointer_iterator& other) const noexcept {
    return (ptr_ - other.ptr_) <= 0;
  }

  constexpr pointer_iterator operator+(diff_t delta) const noexcept {
    return pointer_iterator(ptr_ + delta);
  }

  constexpr pointer_iterator operator-(diff_t delta) const noexcept {
    return pointer_iterator(ptr_ - delta);
  }

  constexpr diff_t operator-(const pointer_iterator& other) const noexcept {
    return ptr_ - other.ptr_;
  }

  constexpr pointer_iterator& operator+=(diff_t delta) noexcept {
    ptr_ += delta;
    return *this;
  }

  constexpr pointer_iterator& operator-=(diff_t delta) noexcept {
    ptr_ -= delta;
    return *this;
  }

  constexpr pointer_iterator operator++(int) noexcept { return pointer_iterator(ptr_++); }
  constexpr pointer_iterator operator--(int) noexcept { return pointer_iterator(ptr_--); }

  constexpr pointer_iterator& operator++() noexcept {
    ++ptr_;
    return *this;
  }

  constexpr pointer_iterator& operator--() noexcept {
    --ptr_;
    return *this;
  }

  constexpr T operator[](diff_t delta) noexcept {
    return *(ptr_ + delta);
  }

  constexpr operator pointer_iterator<const T, Container>() const noexcept {
    return pointer_iterator<const T, Container>(ptr_);
  }

 protected:
  T* ptr_;
};
}  // namespace s21
#endif  // SP_CONTAINERS_POINTER_ITERATOR_H_
