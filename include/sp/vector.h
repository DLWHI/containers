#ifndef SP_CONTAINERS_VECTOR_H_
#define SP_CONTAINERS_VECTOR_H_

#include <sp/pointer_iterator.h>  // iterator and std::distance
#include <sp/reverse_iterator.h>

#include <cstdint>      // int64_t
#include <ostream>      // operator<<
#include <stdexcept>    // exceptions
#include <type_traits>  // as name suggests
#include <utility>      // std::forward, std::swap, std::move

namespace sp {
// T type must meet requirements of Erasable
// Allocator type must meet requirements of Allocator
// Methods may have additional reuirements on types
template <typename T, class Allocator = std::allocator<T>>
class vector {
  using al_traits = std::allocator_traits<Allocator>;

 public:
  using value_type = T;
  using pointer = T*;
  using const_pointer = const T*;
  using reference = T&;
  using const_reference = const T&;
  using size_type = int64_t;
  using difference_type = int64_t;

  using allocator_type = Allocator;

  using iterator = sp::pointer_iterator<T, vector>;
  using const_iterator = sp::pointer_iterator<const T, vector>;
  using reverse_iterator = sp::reverse_iterator<iterator>;
  using const_reverse_iterator = sp::reverse_iterator<const_iterator>;

  static constexpr size_type kCapMul = 2;

  // T has no additional requirements
  // Allocator type must meet additional requirements of DefaultConstructible
  constexpr vector() noexcept(
      std::is_nothrow_default_constructible<Allocator>::value)
      : size_(0), buf_(&al_) {}

  // No additional requirements on template types
  constexpr explicit vector(const Allocator& al) noexcept
      : size_(0), al_(al), buf_(&al_) {}

  // T must meet additional requirements of DefaultInsertable into *this
  constexpr explicit vector(size_type size, const Allocator& al = Allocator())
      : size_(size), al_(al), buf_(size, &al_) {
    fill(buf_.ptr, size_);
  }

  // T must meet additional requirements of CopyInsertable into *this
  constexpr explicit vector(size_type size, const_reference value,
                            const Allocator& al = Allocator())
      : size_(size), al_(al), buf_(size, &al_) {
    fill(buf_.ptr, size_, value);
  };

  // T must meet additional requirements of
  // EmplaceConstructible and
  // MoveInsertable (if InputIterator does not meet
  // the ForwardIterator requirements) into *this
  template <typename InputIterator>
  constexpr vector(const InputIterator& first, const InputIterator& last,
                   const Allocator& al = Allocator())
      : size_(std::distance(first, last)), al_(al), buf_(size_, &al_) {
    fill(buf_.ptr, first, last);
  }

  // T must meet additional requirements of EmplaceConstructible
  constexpr vector(std::initializer_list<value_type> values,
                   const Allocator& al = Allocator())
      : size_(values.size()), al_(al), buf_(size_, &al_) {
    fill(buf_.ptr, values.begin(), values.end());
  }

  // T must meet additional requirements of CopyInsertable into *this
  constexpr vector(const vector& other)
      : size_(other.size_),
        al_(al_traits::select_on_container_copy_construction(other.al_)),
        buf_(other.buf_.cap, &al_) {
    fill(buf_.ptr, other.begin(), other.end());
  }

  // T must meet additional requirements of CopyInsertable into *this
  constexpr vector(const vector& other, const Allocator& al)
      : size_(other.size_), al_(al), buf_(size_, &al_) {
    fill(buf_.ptr, other.buf_.ptr, other.buf_.ptr + other.size_);
  };

  // No additional requirements on template types
  constexpr vector(vector&& other) noexcept : size_(other.size_) {
    if constexpr (!al_traits::is_always_equal::value) {
      al_ = std::move(other.al_);
    }
    buf_.alc = &al_;
    buf_.ptr = other.buf_.ptr;
    buf_.cap = other.buf_.cap;
    other.buf_.ptr = nullptr;
    other.buf_.cap = 0;
    other.size_ = 0;
  }

  // T must meet additional requirements of MoveInsertable into *this
  constexpr vector(vector&& other, const Allocator& al) noexcept(
      al_traits::is_always_equal::value)
      : size_(0), al_(al), buf_(0, &al_) {
    if (al == other.al_) {
      swap(other);
    } else {
      pointer_buffer temp(other.size_, &al_);
      move_from(temp.ptr, other.buf_.ptr, other.size_);
      buf_.swap(temp);
      size_ = other.size_;
    }
  }

  // T must meet additional requirements of
  //  CopyInsertable and CopyAssignable into *this
  constexpr vector& operator=(const vector& other) {
    if constexpr (al_traits::propagate_on_container_copy_assignment::value) {
      pointer_buffer temp(&al_);
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
      size_ = 0;
      al_ = other.al_;
    }
    if (other.size_ > buf_.cap || !std::is_nothrow_copy_assignable<T>::value) {
      pointer_buffer temp(other.buf_.cap, &al_, buf_.ptr);
      fill(temp.ptr, other.buf_.ptr, other.buf_.ptr + other.size_);
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
    } else if constexpr (std::is_nothrow_copy_assignable<T>::value) {
      if (size_ < other.size_) {
        fill(buf_.ptr + size_, other.buf_.ptr + size_,
             other.buf_.ptr + other.size_);
      } else {
        destroy_content(buf_.ptr + other.size_, size_ - other.size_);
      }
      for (size_type i = 0; i < other.size_ && i < size_; ++i) {
        buf_.ptr[i] = other.buf_.ptr[i];
      }
    }
    size_ = other.size_;
    return *this;
  }

  // T must meet additional requirements of
  //  MoveInsertable into *this and MoveAssignable if
  //  allocator_traits<Allocator>
  //  ::propagate_on_container_move_assignment::value is false
  constexpr vector& operator=(vector&& other) noexcept(
      al_traits::propagate_on_container_move_assignment::value ||
      al_traits::is_always_equal::value) {
    if constexpr (al_traits::propagate_on_container_move_assignment::value) {
      al_ = other.al_;
    }
    if (other.size_ > buf_.cap || !std::is_nothrow_move_assignable<T>::value) {
      pointer_buffer temp(other.buf_.cap, &al_, buf_.ptr);
      move_from(temp.ptr, other.buf_.ptr, other.size_);
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
    } else if constexpr (std::is_nothrow_move_assignable<T>::value) {
      move_from(buf_.ptr + size_, other.buf_.ptr + other.size_,
                other.size_ - size_);
      destroy_content(buf_.ptr + other.size_, size_ - other.size_);
      for (size_type i = 0; i < other.size_ && i < size_; ++i) {
        buf_.ptr[i] = std::move(other.buf_.ptr[i]);
      }
    }
    size_ = other.size_;
    return *this;
  }

  // No additional requirements on template types
  constexpr virtual ~vector() noexcept { clear(); }

  //============================================================================
  // No additional requirements on template types for all methods below

  constexpr allocator_type get_allocator() const noexcept { return al_; }

  constexpr reference at(size_type pos) {
    return (0 <= pos && pos < size_)
               ? buf_.ptr[pos]
               : throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr const_reference at(size_type pos) const {
    return (0 <= pos && pos < size_)
               ? buf_.ptr[pos]
               : throw std::out_of_range("Accessing element out of bounds");
  }

  constexpr reference front() noexcept { return buf_.ptr[0]; }
  constexpr reference back() noexcept { return buf_.ptr[size_ - 1]; }
  constexpr pointer data() noexcept { return buf_.ptr; }

  constexpr const_reference front() const noexcept { return buf_.ptr[0]; }
  constexpr const_reference back() const noexcept {
    return buf_.ptr[size_ - 1];
  }
  constexpr const_pointer data() const noexcept { return buf_.ptr; }

  constexpr iterator begin() noexcept { return iterator(buf_.ptr); }
  constexpr const_iterator begin() const noexcept {
    return const_iterator(buf_.ptr);
  }
  constexpr const_iterator cbegin() const noexcept {
    return const_iterator(buf_.ptr);
  }

  constexpr iterator end() noexcept { return iterator(buf_.ptr + size_); }
  constexpr const_iterator end() const noexcept {
    return const_iterator(buf_.ptr + size_);
  }
  constexpr const_iterator cend() const noexcept {
    return const_iterator(buf_.ptr + size_);
  }

  constexpr reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }
  constexpr const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(end());
  }
  constexpr const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }

  constexpr reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }
  constexpr const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(begin());
  }
  constexpr const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(begin());
  }

  constexpr bool empty() const noexcept { return !size_; }
  constexpr size_type size() const noexcept { return size_; }
  constexpr size_type capacity() const noexcept { return buf_.cap; }
  constexpr size_type max_size() const noexcept {
    return al_traits::max_size(al_);
  }
  //============================================================================

  // T must meet additional requirement of CopyAssignable and
  //  CopyInsertable into *this
  constexpr void assign(size_type count, const_reference value) {
    if (count > max_size() || count < 0) {
      throw std::length_error("Invalid count provided");
    }
    if (count > buf_.cap) {
      pointer_buffer temp(std::max(kCapMul * buf_.cap, count), &al_, buf_.ptr);
      fill(temp.ptr, count, value);
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
      size_ = count;
    } else {
      for (size_type i = 0; i < count && i < size_; ++i) {
        buf_.ptr[i] = value;
      }
      move_end(count - size_, value);
    }
  }

  // T must meet additional requirement of EmplaceConstructible into *this
  //  and assignable from InputIterator (I guess CopyAssignable would be okay)
  //  and MoveInsertable if InputIterator does not satisfy ForwardIterator
  template <typename InputIterator>
  constexpr void assign(InputIterator first, InputIterator last) {
    size_type count = std::distance(first, last);
    if (count > max_size() || count < 0) {
      throw std::length_error("Invalid or too big range provided");
    }
    if (count > buf_.cap) {
      pointer_buffer temp(std::max(kCapMul * buf_.cap, count), &al_, buf_.ptr);
      fill(temp.ptr, first, last);
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
    } else {
      for (size_type i = 0; i < count && i < size_; ++i, ++first) {
        buf_.ptr[i] = *first;
      }
      if (first != last) {
        fill(buf_.ptr + size_, first, last);
      } else {
        destroy_content(buf_.ptr + count, size_ - count);
      }
    }
    size_ = count;
  }

  // Same as assign(values.begin(), values.end())
  constexpr void assign(std::initializer_list<value_type> values) {
    return assign(values.begin(), values.end());
  }

  // T must meet additional requirements of MoveInsertable into *this
  constexpr void reserve(size_type count) {
    if (count > max_size() || count < 0) {
      throw std::length_error("Invalid reserve space");
    } else if (count > buf_.cap) {
      resize_buffer(count);
    }
  }

  // T must meet additional requirements of MoveInsertable into *this
  constexpr void shrink_to_fit() {
    if (buf_.cap > size_) {
      resize_buffer(size_);
    }
  }

  // No additional requirements on template types
  constexpr void clear() noexcept {
    for (; size_; --size_) {
      al_traits::destroy(al_, buf_.ptr + size_ - 1);
    }
  }

  // T must meet additional requirements of
  //  MoveInsertable and DefaultInsertable into *this
  constexpr void resize(size_type count) {
    if (count > max_size() || count < 0) {
      throw std::length_error("Invalid count provided");
    }
    if (count == size_) {
      return;
    } else if (count >= buf_.cap) {
      pointer_buffer temp(count, &al_, buf_.ptr);
      move_from(temp.ptr, buf_.ptr, size_);
      try {
        fill(temp.ptr + size_, count - size_);
      } catch (...) {
        destroy_content(temp.ptr, size_);
        throw;
      }
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
      size_ = count;
    } else {
      move_end(count - size_);
    }
  }

  // T must meet additional requirements of CopyInsertable into *this
  constexpr void resize(size_type count, const_reference value) {
    if (count > max_size() || count < 0) {
      throw std::length_error("Invalid count provided");
    }
    if (count == size_) {
      return;
    } else if (count >= buf_.cap) {
      pointer_buffer temp(count, &al_, buf_.ptr);
      move_from(temp.ptr, buf_.ptr, size_);
      try {
        fill(temp.ptr + size_, count - size_, value);
      } catch (...) {
        destroy_content(temp.ptr, size_);
        throw;
      }
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
      size_ = count;
    } else {
      move_end(count - size_, value);
    }
  }
  //==============================================================================

  // T must meet additional requirements of CopyInsertable
  constexpr void push_back(const_reference value) { emplace_back(value); }

  // T must meet additional requirements of MoveInsertable
  constexpr void push_back(value_type&& value) {
    emplace_back(std::forward<value_type>(value));
  }

  // No additional requirements on template types
  constexpr void pop_back() noexcept(std::is_nothrow_destructible<T>::value) {
    al_traits::destroy(al_, buf_.ptr + size_ - 1);
    --size_;
  }

  // T must meet additional requirements of CopyAssignable
  //   and CopyInsertable into *this
  constexpr iterator insert(const_iterator pos, const_reference value) {
    return insert(pos, 1, value);
  }

  // T must meet additional requirements of MoveAssignable
  //   and MoveInsertable into *this
  constexpr iterator insert(const_iterator pos, value_type&& value) {
    return emplace(pos, std::forward<value_type>(value));
  }

  // T must meet additional requirements of CopyAssignable
  //   and CopyInsertable into *this
  constexpr iterator insert(const_iterator pos, size_type count,
                            const_reference value) {
    size_type ind = pos - begin();
    if (size_ + count >= buf_.cap || !std::is_nothrow_swappable<T>::value) {
      pointer_buffer temp(std::max(kCapMul * buf_.cap, size_ + count), &al_,
                          buf_.ptr);
      fill(temp.ptr + ind, count, value);
      try {
        split_buffer(temp.ptr, ind, count);
      } catch (...) {
        destroy_content(temp.ptr + ind, count);
        throw;
      }
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
    } else if constexpr (std::is_nothrow_swappable<T>::value) {
      fill(buf_.ptr + size_, count, value);
      std::reverse(buf_.ptr + ind, buf_.ptr + size_ + count);
      std::reverse(buf_.ptr + ind, buf_.ptr + ind + count);
      std::reverse(buf_.ptr + ind + count, buf_.ptr + size_ + count);
    }
    size_ += count;
    return begin() + ind;
  }

  // T must meet additional requirements of Swappable, MoveAssignable,
  //   MoveConstructible, EmplaceConstructible and MoveInsertable into *this
  template <typename InputIt>
  constexpr iterator insert(const_iterator pos, InputIt first, InputIt last) {
    size_type ind = pos - begin();
    size_type count = std::distance(first, last);
    if (count + size_ >= buf_.cap || !std::is_nothrow_swappable<T>::value) {
      pointer_buffer temp(std::max(kCapMul * buf_.cap, size_ + count), &al_,
                          buf_.ptr);
      fill(temp.ptr + ind, first, last);
      try {
        split_buffer(temp.ptr, ind, count);
      } catch (...) {
        destroy_content(temp.ptr + ind, count);
        throw;
      }
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
    } else if constexpr (std::is_nothrow_swappable<T>::value) {
      fill(buf_.ptr + size_, first, last);
      std::reverse(buf_.ptr + ind, buf_.ptr + size_ + count);
      std::reverse(buf_.ptr + ind, buf_.ptr + ind + count);
      std::reverse(buf_.ptr + ind + count, buf_.ptr + size_ + count);
    }
    size_ += count;
    return begin() + ind;
  }

  // Same as insert(pos, values.begin(), values.end())
  constexpr iterator insert(const_iterator pos,
                            std::initializer_list<T> values) {
    return insert(pos, values.begin(), values.end());
  }

  // T must meet additional requirements of MoveAssignable
  constexpr iterator erase(const_iterator pos) noexcept(
      std::is_nothrow_move_assignable<T>::value) {
    size_type ind = pos - begin();
    if constexpr (std::is_nothrow_move_assignable<T>::value) {
      for (size_type i = ind; i < size_ - 1; ++i) {
        buf_.ptr[i] = std::move(buf_.ptr[i + 1]);
      }
      al_traits::destroy(al_, buf_.ptr + size_ - 1);
    } else {
      pointer_buffer temp(buf_.cap, &al_, buf_.ptr);
      split_buffer(temp.ptr, ind + 1, -1);
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
    }
    --size_;
    return begin() + ind;
  }

  // T must meet additional requirements of MoveAssignable
  constexpr iterator erase(const_iterator first, const_iterator last) noexcept(
      std::is_nothrow_move_assignable<T>::value) {
    size_type start = first - begin();
    size_type finish = last - begin();
    size_type count = finish - start;
    if constexpr (std::is_nothrow_move_assignable<T>::value) {
      for (size_type i = start; i < size_ - count; ++i) {
        buf_.ptr[i] = std::move(buf_.ptr[i + count]);
      }
      destroy_content(buf_.ptr + size_ - count, count);
    } else {
      pointer_buffer temp(buf_.cap, &al_, buf_.ptr);
      split_buffer(temp.ptr, finish, -count);
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
    }
    size_ -= count;
    return begin() + start;
  }

  // T must meet additional requirements of EmplaceConstrutible from args,
  //  MoveAssignable and MoveInsertable into *this
  template <typename... Args>
  constexpr iterator emplace(const_iterator pos, Args&&... args) {
    size_type ind = pos - begin();
    if (size_ == buf_.cap || !std::is_nothrow_move_assignable<T>::value) {
      pointer_buffer temp(std::max(kCapMul * buf_.cap, size_type(1)), &al_,
                          buf_.ptr);
      al_traits::construct(al_, temp.ptr + ind, std::forward<Args>(args)...);
      try {
        split_buffer(temp.ptr, ind, 1);
      } catch (...) {
        al_traits::destroy(al_, temp.ptr + ind);
        throw;
      }
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
    } else if constexpr (std::is_nothrow_move_assignable<T>::value) {
      T emp(std::forward<Args>(args)...);
      al_traits::construct(al_, buf_.ptr + size_,
                           std::move(buf_.ptr[size_ - 1]));
      for (size_type i = size_ - 1; i > ind; --i) {
        buf_.ptr[i] = std::move(buf_.ptr[i - 1]);
      }
      buf_.ptr[ind] = std::move(emp);
    }
    ++size_;
    return begin() + ind;
  }

  // T must meet additional requirements of EmplaceConstrutible from args
  //   and MoveInsertable into *this
  template <typename... Args>
  constexpr T& emplace_back(Args&&... args) {
    if (size_ >= buf_.cap) {
      pointer_buffer temp(std::max(kCapMul * buf_.cap, size_type(1)), &al_,
                          buf_.ptr);
      al_traits::construct(al_, temp.ptr + size_, std::forward<Args>(args)...);
      try {
        move_from(temp.ptr, buf_.ptr, size_);
      } catch (...) {
        al_traits::destroy(al_, temp.ptr + size_);
        throw;
      }
      buf_.swap(temp);
      destroy_content(temp.ptr, size_);
    } else {
      al_traits::construct(al_, buf_.ptr + size_, std::forward<Args>(args)...);
    }
    ++size_;
    return buf_.ptr[size_ - 1];
  }

  // No additional requirements on types
  constexpr void swap(vector& other) noexcept(
      al_traits::propagate_on_container_swap::value ||
      al_traits::is_always_equal::value) {
    if constexpr (al_traits::propagate_on_container_swap::value ||
                  !al_traits::is_always_equal::value) {
      using std::swap; 
      swap(al_, other.al_);
    }
    buf_.swap(other.buf_);
    std::swap(size_, other.size_);
  }

  // T must meet additional requirements of EqualityComparable
  constexpr bool operator==(const vector& other) const noexcept {
    if (size_ != other.size_) return false;
    for (size_type i = 0; i < size_; ++i)
      if (buf_.ptr[i] != other.buf_.ptr[i]) return false;
    return true;
  }

  // T must meet additional requirements of EqualityComparable
  constexpr bool operator!=(const vector& other) const noexcept {
    return !(*this == other);
  }

  // No additional requirements on types
  constexpr reference operator[](size_type index) noexcept {
    return buf_.ptr[index];
  }

  // No additional requirements on types
  constexpr const_reference operator[](size_type index) const noexcept {
    return buf_.ptr[index];
  }

  // I guess os << T must be valid
  friend std::ostream& operator<<(std::ostream& os, const vector& vec) {
    for (size_type i = 0; i < vec.size_ - 1; ++i) os << vec.buf_.ptr[i] << ' ';
    os << vec.buf_.ptr[vec.size_ - 1];
    return os;
  }

 private:
  struct pointer_buffer {
    constexpr explicit pointer_buffer(Allocator* al = nullptr)
        : alc(al), ptr(nullptr), cap(0) {}
    constexpr pointer_buffer(size_type size, Allocator* al,
                             pointer hint = nullptr)
        : alc(al), cap(size) {
      if (cap < 0) {
        throw std::invalid_argument("Invalid memory buffer length");
      }
      ptr = (cap) ? al_traits::allocate(*alc, size, hint) : nullptr;
    }
    constexpr pointer_buffer(const pointer_buffer&) = delete;
    constexpr pointer_buffer(pointer_buffer&& other) = delete;
    constexpr pointer_buffer& operator=(const pointer_buffer&) = delete;
    constexpr pointer_buffer& operator=(pointer_buffer&& other) = delete;
    constexpr ~pointer_buffer() { al_traits::deallocate(*alc, ptr, cap); }

    constexpr void swap(pointer_buffer& other) noexcept {
      std::swap(ptr, other.ptr);
      std::swap(cap, other.cap);
    }

    Allocator* alc;
    pointer ptr;
    size_type cap;
  };

  template <typename... Args>
  constexpr void fill(pointer arr, size_type count, Args&&... args) noexcept(
      std::is_nothrow_constructible<T, Args...>::value) {
    size_type i = 0;
    try {
      for (; i < count; ++i) {
        al_traits::construct(al_, arr + i, std::forward<Args>(args)...);
      }
    } catch (...) { 
      for (; i; --i) {
        al_traits::destroy(al_, arr + i - 1);
      }
      if constexpr (!std::is_nothrow_constructible<T, Args...>::value) throw;
    }
  }

  template <typename InIt>
  constexpr void fill(pointer arr, InIt first, InIt last) noexcept(
      std::is_nothrow_copy_constructible<T>::value) {
    InIt i = first;
    try {
      for (; i != last; ++i, ++arr) al_traits::construct(al_, arr, *i);
    } catch (...) {
      for (; i != first; --i) al_traits::destroy(al_, --arr);
      if constexpr (!std::is_nothrow_copy_constructible<T>::value) throw;
    }
  }

  constexpr void move_from(
      pointer dest, pointer source,
      size_type count) noexcept(std::is_nothrow_move_constructible<T>::value) {
    size_type i = 0;
    if constexpr (std::is_nothrow_move_constructible<T>::value) {
      for (; i < count; ++i)
        al_traits::construct(al_, dest + i, std::move(source[i]));
    } else {
      try {
        for (; i < count; ++i) al_traits::construct(al_, dest + i, source[i]);
      } catch (...) {
        for (; i; --i) al_traits::destroy(al_, dest + i - 1);
        throw;
      }
    }
  }

  constexpr void resize_buffer(size_type n_size) {
    pointer_buffer temp(n_size, &al_);
    move_from(temp.ptr, buf_.ptr, size_);
    buf_.swap(temp);
    destroy_content(temp.ptr, size_);
  }

  template <typename... Args>
  constexpr void move_end(size_type offset, Args&&... append_args) {
    if (offset < 0) {
      destroy_content(buf_.ptr + size_ + offset, -offset);
    } else {
      fill(buf_.ptr + size_, offset, std::forward<Args>(append_args)...);
    }
    size_ += offset;
  }

  constexpr void split_buffer(
      pointer dest, size_type ind,
      size_type offset) noexcept(std::is_nothrow_move_constructible<T>::value) {
    size_type lim = std::min(ind, ind + offset);
    move_from(dest, buf_.ptr, lim);
    try {
      move_from(dest + ind + offset, buf_.ptr + ind, size_ - ind);
    } catch (...) {
      destroy_content(dest, lim);
      if constexpr (!std::is_nothrow_move_constructible<T>::value) throw;
    }
  }

  constexpr void destroy_content(pointer ptr, size_type count) noexcept(
      std::is_nothrow_destructible<T>::value) {
    for (; count; --count) {
      al_traits::destroy(al_, ptr + count - 1);
    }
  }

  size_type size_;
  allocator_type al_;
  pointer_buffer buf_;
};
}  // namespace sp
#endif  // SP_CONTAINERS_VECTOR_H_
