#ifndef SRC_CONTAINERS_S21_ARRAY_H_
#define SRC_CONTAINERS_S21_ARRAY_H_
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <type_traits>

#include "s21_iterator.h"

namespace s21 {
typedef uint32_t size_t;

// I question this variant of array implementation, but this implementation
// is almost same as std::array.
// For those who question constructor absence:
//   As we do not declare any of the constructors/destructors, they are being
//   implicitly defined by compiler, thus there is no violation of rule of 5
// T is reuired to be Swappable, CopyAssignable
template <typename T, size_t N>
struct array {
  typedef T value_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef size_t size_type;
  typedef iterator<T, array> v_iterator;
  typedef iterator<const T, array> v_const_iterator;
  typedef reverse_iterator<T, array> v_reverse_iterator;
  typedef reverse_iterator<const T, array> v_creverse_iterator;

  constexpr reference at(size_t pos) {
    if (pos >= N) throw std::out_of_range("Accessing element out of bounds");
    return arr[pos];
  }

  constexpr const_reference at(size_t pos) const {
    if (pos >= N) throw std::out_of_range("Accessing element out of bounds");
    return arr[pos];
  }

  constexpr reference front() { return arr[0]; }
  constexpr reference back() { return arr[N - 1]; }
  constexpr pointer data() noexcept { return arr; }

  constexpr const_reference front() const { return arr[0]; }
  constexpr const_reference back() const { return arr[N - 1]; }
  constexpr const_pointer data() const noexcept { return arr; }

  constexpr bool empty() const noexcept { return false; }
  constexpr size_t size() const noexcept { return N; }
  constexpr size_t max_size() const noexcept { return N; }

  constexpr v_iterator begin() noexcept { return v_iterator(arr); }
  constexpr v_const_iterator begin() const noexcept { return cbegin(); }
  constexpr v_const_iterator cbegin() const noexcept {
    return v_const_iterator(arr);
  }
  constexpr v_iterator end() noexcept { return v_iterator(arr + N); }
  constexpr v_const_iterator end() const noexcept { return cend(); }
  constexpr v_const_iterator cend() const noexcept {
    return v_const_iterator(arr + N);
  }

  constexpr v_reverse_iterator rbegin() noexcept {
    return v_reverse_iterator(arr + N - 1);
  }
  constexpr v_creverse_iterator rbegin() const noexcept { return crbegin(); }
  constexpr v_creverse_iterator crbegin() const noexcept {
    return v_creverse_iterator(arr + N - 1);
  }
  constexpr v_reverse_iterator rend() noexcept {
    return v_reverse_iterator(arr - 1);
  }
  constexpr v_creverse_iterator rend() const noexcept { return crend(); }
  constexpr v_creverse_iterator crend() const noexcept {
    return v_creverse_iterator(arr - 1);
  }

  constexpr void fill(const_reference value) {
    for (size_t i = 0; i < N; i++) {
      arr[i] = value;
    }
  }

  constexpr void swap(array& other) noexcept(std::is_nothrow_swappable_v<T>) {
    std::swap_ranges(arr, arr + size(), other.arr);
  }

  constexpr reference operator[](size_t i) { return arr[i]; }
  constexpr const_reference operator[](size_t i) const { return arr[i]; }
  constexpr bool operator==(const array& other) const {
    if (N != other.size()) return false;
    for (size_t i = 0; i < N; i++)
      if (arr[i] != other.arr[i]) return false;
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os, const array& target) {
    for (size_t i = 0; i < N; i++) os << target.arr[i] << ' ';
    return os;
  }

  value_type arr[N];
};
}  // namespace s21
#endif  // SRC_CONTAINERS_S21_ARRAY_H_
