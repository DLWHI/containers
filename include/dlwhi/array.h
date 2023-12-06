#ifndef DLWHI_CONTAINERS_ARRAY_H_
#define DLWHI_CONTAINERS_ARRAY_H_
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <type_traits>

#include "pointer_iterator.h"
#include "reverse_iterator.h"

namespace sp {

using size_t = int64_t;

// Requires T to be Swappable, CopyAssignable
template <typename T, size_t N>
struct array {
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_t = int64_t;

  using iterator = sp::pointer_iterator<T, array>;
  using const_iterator = sp::pointer_iterator<const T, array>;
  using reverse_iterator = sp::reverse_iterator<iterator>;
  using const_reverse_iterator = sp::reverse_iterator<const_iterator>;

  constexpr reference at(size_t pos) {
    return (0 <= pos && pos < N) ? arr[pos] : throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr const_reference at(size_t pos) const {
    return (0 <= pos && pos < N) ? arr[pos] : throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr reference front() { return arr[0]; }
  constexpr reference back() { return arr[N - 1]; }
  constexpr pointer data() noexcept { return arr; }

  constexpr const_reference front() const { return arr[0]; }
  constexpr const_reference back() const { return arr[N - 1]; }
  constexpr const_pointer data() const noexcept { return arr; }

  constexpr bool empty() const noexcept { return !N; }
  constexpr size_t size() const noexcept { return N; }
  constexpr size_t max_size() const noexcept { return N; }

  constexpr iterator begin() noexcept { return iterator(arr); }
  constexpr const_iterator begin() const noexcept { return cbegin(); }
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator(arr);
  }
  constexpr iterator end() noexcept { return iterator(arr + N); }
  constexpr const_iterator end() const noexcept { return cend(); }
  constexpr const_iterator cend() const noexcept {
    return const_iterator(arr + N);
  }

  constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(arr + N - 1);
  }
  constexpr const_reverse_iterator rbegin() const noexcept { return crbegin(); }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(arr + N - 1);
  }
  constexpr reverse_iterator rend() noexcept {
    return reverse_iterator(arr - 1);
  }
  constexpr const_reverse_iterator rend() const noexcept { return crend(); }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(arr - 1);
  }

  constexpr void fill(const_reference value) noexcept(std::is_nothrow_copy_assignable<T>::value) {
    for (size_t i = 0; i < N; ++i) {
      arr[i] = value;
    }
  }

  constexpr void swap(array& other) noexcept(std::is_nothrow_swappable<T>::value) {
    std::swap_ranges(arr, arr + size(), other.arr);
  }

  constexpr reference operator[](size_t i) { return arr[i]; }
  constexpr const_reference operator[](size_t i) const { return arr[i]; }
  constexpr bool operator==(const array& other) const {
    for (size_t i = 0; i < N; ++i)
      if (arr[i] != other.arr[i]) return false;
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os, const array& array) {
    for (size_t i = 0; i < N - 1; ++i) os << array.arr[i] << ' ';
    os << array.arr[N - 1];
    return os;
  }

  value_type arr[N];
};

template <typename... Vals>
constexpr array<typename std::decay<typename std::common_type<Vals...>::type>::type, 
                sizeof...(Vals)> 
make_array(Vals&&... values) {
    return array<typename std::decay<typename std::common_type<Vals...>::type>::type,
                 sizeof...(Vals)>
                 {std::forward<Vals>(values)...};
}

template <typename T>
struct array<T, 0> {
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_t = int64_t;

  using iterator = sp::pointer_iterator<T, array>;
  using const_iterator = sp::pointer_iterator<const T, array>;
  using reverse_iterator = sp::reverse_iterator<iterator>;
  using const_reverse_iterator = sp::reverse_iterator<const_iterator>;

  constexpr const_reference at(size_t pos) const {
    (void) pos;
    throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr const_reference front() const { return *begin(); }
  constexpr const_reference back() const { return *begin(); }
  constexpr const_pointer data() const noexcept { return nullptr; }

  constexpr bool empty() const noexcept { return true; }
  constexpr size_t size() const noexcept { return 0; }
  constexpr size_t max_size() const noexcept { return 0; }

  constexpr iterator begin() noexcept { return iterator(nullptr); }
  constexpr const_iterator begin() const noexcept { return cbegin(); }
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator(nullptr);
  }
  constexpr iterator end() noexcept { return iterator(nullptr); }
  constexpr const_iterator end() const noexcept { return cend(); }
  constexpr const_iterator cend() const noexcept {
    return const_iterator(nullptr);
  }

  constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(nullptr);
  }
  constexpr const_reverse_iterator rbegin() const noexcept { return crbegin(); }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(nullptr);
  }
  constexpr reverse_iterator rend() noexcept {
    return reverse_iterator(nullptr);
  }
  constexpr const_reverse_iterator rend() const noexcept { return crend(); }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(nullptr);
  }

  constexpr void fill(const_reference value) {
    *begin();
    (void) value;
  }

  constexpr void swap(array& other) noexcept {
    (void) other;
  }

  constexpr reference operator[](size_t i) { (void)i; return *begin(); }
  constexpr const_reference operator[](size_t i) const { (void)i; return *begin(); }
  constexpr bool operator==(const array& other) const {
    (void) other;
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os, const array& array) noexcept {
    (void) array;
    return os;
  }
};


}  // namespace s21
#endif  // DLWHI_CONTAINERS_ARRAY_H_
