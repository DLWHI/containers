#ifndef SP_CONTAINERS_ARRAY_H_
#define SP_CONTAINERS_ARRAY_H_

#include <algorithm>
#include <cstdint>
#include <ostream>
#include <stdexcept>
#include <type_traits>

#include "pointer_iterator.h"
#include "reverse_iterator.h"

namespace sp {

using size_t = int64_t;

// Requires N >= 0 (UB otherwise)
// Relies on implicitly declared ctor so thus implies:
//  Copy ctor/assignment operator requiers T to be
//    CopyConstructible and CopyAssignable respectively
//  Move ctor/assignment operator requiers T to be
//    MoveConstructible and MoveAssignable respectively
// Other methods have no requirements on type T
template <typename T, sp::size_t N>
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
    return (0 <= pos && pos < N)
               ? elements[pos]
               : throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr const_reference at(size_t pos) const {
    return (0 <= pos && pos < N)
               ? elements[pos]
               : throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr reference front() { return elements[0]; }
  constexpr reference back() { return elements[N - 1]; }
  constexpr pointer data() noexcept { return elements; }

  constexpr const_reference front() const { return elements[0]; }
  constexpr const_reference back() const { return elements[N - 1]; }
  constexpr const_pointer data() const noexcept { return elements; }

  constexpr bool empty() const noexcept { return false; }
  constexpr size_t size() const noexcept { return N; }
  constexpr size_t max_size() const noexcept { return N; }

  constexpr iterator begin() noexcept { return iterator(elements); }
  constexpr const_iterator begin() const noexcept { return cbegin(); }
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator(elements);
  }
  constexpr iterator end() noexcept { return iterator(elements + N); }
  constexpr const_iterator end() const noexcept { return cend(); }
  constexpr const_iterator cend() const noexcept {
    return const_iterator(elements + N);
  }

  constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(elements + N - 1);
  }
  constexpr const_reverse_iterator rbegin() const noexcept { return crbegin(); }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(elements + N - 1);
  }
  constexpr reverse_iterator rend() noexcept {
    return reverse_iterator(elements - 1);
  }
  constexpr const_reverse_iterator rend() const noexcept { return crend(); }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(elements - 1);
  }

  constexpr void fill(const_reference value) noexcept(
      std::is_nothrow_copy_assignable<T>::value) {
    for (size_t i = 0; i < N; ++i) {
      elements[i] = value;
    }
  }

  constexpr void swap(array& other) noexcept(
      std::is_nothrow_swappable<T>::value) {
    std::swap_ranges(elements, elements + size(), other.elements);
  }

  constexpr reference operator[](size_t i) { return elements[i]; }
  constexpr const_reference operator[](size_t i) const { return elements[i]; }
  constexpr bool operator==(const array& other) const {
    for (size_t i = 0; i < N; ++i)
      if (elements[i] != other.elements[i]) return false;
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os, const array& array) {
    for (size_t i = 0; i < N - 1; ++i) os << array.elements[i] << ' ';
    os << array.elements[N - 1];
    return os;
  }

  value_type elements[N];
};

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
    (void)pos;
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

  constexpr void fill(const_reference value) { (void)value; }

  constexpr void swap(array& other) noexcept { (void)other; }

  constexpr reference operator[](size_t i) {
    (void)i;
    return *begin();
  }
  constexpr const_reference operator[](size_t i) const {
    (void)i;
    return *begin();
  }
  constexpr bool operator==(const array& other) const {
    (void)other;
    return true;
  }

  friend std::ostream& operator<<(std::ostream& os,
                                  const array& array) noexcept {
    (void)array;
    return os;
  }
};

template <class T, sp::size_t N, sp::size_t... Order>
constexpr sp::array<typename std::remove_cv<T>::type, N> to_array(
    T (&a)[N], std::integer_sequence<sp::size_t, Order...>) {
  static_assert(N == sizeof...(Order), "Sequence must have same lenght as array");
  return {{a[Order]...}};
}

template <class T, sp::size_t N>
constexpr sp::array<typename std::remove_cv<T>::type, N> to_array(T (&a)[N]) {
  return to_array(a, std::make_integer_sequence<sp::size_t, N>{});
}

template <class T, sp::size_t N, sp::size_t... Order>
constexpr sp::array<typename std::remove_cv<T>::type, N> to_array(
    T (&&a)[N], std::integer_sequence<sp::size_t, Order...>) {
  static_assert(N == sizeof...(Order), "Sequence must have same lenght as array");
  return {{std::move(a[Order])...}};
}

template <class T, sp::size_t N>
constexpr sp::array<typename std::remove_cv<T>::type, N> to_array(T (&&a)[N]) {
  return to_array(std::move(a), std::make_integer_sequence<sp::size_t, N>{});
}

}  // namespace sp
#endif  // SP_CONTAINERS_ARRAY_H_
