#ifndef SP_TESTS_TEST_HELPERS_H_
#define SP_TESTS_TEST_HELPERS_H_

#include <memory>
#include <stdexcept>
#include <string>

enum class constructed { kDef, kParam, kCopy, kMove };

class safe {
 public:
  safe() : birth(constructed::kDef), id_("default"), leak_(new int()){}
  explicit safe(const std::string& name)
      : birth(constructed::kParam), id_(name), leak_(new int()){}
  safe(safe&& other) noexcept : birth(constructed::kMove) {
    id_ = std::move(other.id_);
    leak_ = other.leak_;
    other.leak_ = nullptr;
  }
  safe(const safe& other)
      : birth(constructed::kCopy), id_(other.id_), leak_(new int()){}
  safe& operator=(const safe& other) { 
    id_ = other.id_;
    return *this; 
  }
  safe& operator=(safe&& other) noexcept { 
    id_ = std::move(other.id_);
    return *this; 
  }
  virtual ~safe() { delete leak_; }

  bool operator==(const safe& other) const { return other.id_ == id_; }
  bool operator!=(const safe& other) const { return other.id_ != id_; }

  friend std::ostream& operator<<(std::ostream& os, const safe& obj) {
    os << obj.id_;
    return os;
  }
  const constructed birth;

 protected:
  std::string id_;
  int* leak_;
};

// + has safe Move Constructor/Assignment op dummy
class not_safe : public safe {
 public:
  not_safe() : safe(){}
  explicit not_safe(const std::string& name) : safe(name){}
  not_safe(const not_safe& other) : safe(other){}
  not_safe(not_safe&& other) : safe(other) {}
  not_safe& operator=(const not_safe& other) { 
    id_ = other.id_;
    return *this; 
  }
  not_safe& operator=(not_safe&& other) { 
    id_ = std::move(other.id_);
    return *this; 
  }
  virtual ~not_safe() {}
};

// exception throwing dummy
class throwing : public safe {
 public:
  throwing() : safe(){}
  explicit throwing(const std::string& name) : safe(name){}
  throwing(throwing&& other) : safe(std::move(other)) {
    if (!(++count % 5)) {
      throw std::runtime_error("IN YOUR FUCKING FACE");
    } 
    id_ = std::move(other.id_);
  };
  throwing(const throwing& other) : safe(other) {
    if (!(++count % 5)) {
      throw std::runtime_error("IN YOUR FUCKING FACE");
    }
  };
  throwing& operator=(const throwing& other) { 
    if (!(++count % 5)) {
      throw std::runtime_error("IN YOUR FUCKING FACE");
    }
    id_ = other.id_;
    return *this;
  }
  throwing& operator=(throwing&& other) {
    if (!(++count % 5)) {
      throw std::runtime_error("IN YOUR FUCKING FACE");
    } 
    id_ = std::move(other.id_);
    return *this;
  }
  virtual ~throwing() = default;

  friend std::ostream& operator<<(std::ostream& os, const throwing& obj) {
    os << obj.id_;
    return os;
  }

  static int count;
};
int throwing::count = 0;

// - DefaultConstructible dummy
class no_def : public safe {
 public:
  explicit no_def(const std::string& name) : safe(name){}
  // no_def(no_def&& other) noexcept : safe(std::move(other)) {}
  no_def(const no_def& other) : safe(other){}
  // no_def& operator=(const no_def& other) { id_ = other.id_; }
  // no_def& operator=(no_def&& other) { id_ = std::move(other.id_); }
  virtual ~no_def() = default;
};

// // + safe assignment operators
// class safe_assign : public safe {
//  public:
//   safe_assign() : safe() {}
//   explicit safe_assign(const std::string& name) : safe(name) {}
//   safe_assign(const safe_assign& other) : safe(other) {}
//   safe_assign(safe_assign&& other) : safe(other) {}
//   safe_assign& operator=(const safe_assign& other) { 
//     id_ = other.id_;
//     return *this; 
//   }
//   safe_assign& operator=(safe_assign&& other) noexcept { 
//     id_ = std::move(other.id_);
//     return *this;
//   }
//   virtual ~safe_assign() {}
// };

// // + not safe assignment operators
// class not_safe_assign : public safe {
//  public:
//   not_safe_assign() : safe() {}
//   explicit not_safe_assign(const std::string& name) : safe(name) {}
//   not_safe_assign(const not_safe_assign& other) : safe(other) {}
//   not_safe_assign(not_safe_assign&& other) : safe(other) {}
//   not_safe_assign& operator=(const not_safe_assign& other) { 
//     id_ = other.id_;
//     return *this; 
//   }
//   not_safe_assign& operator=(not_safe_assign&& other) { 
//     id_ = std::move(other.id_);
//     return *this;
//   }
//   virtual ~not_safe_assign() {}
// };

// state allocator
template <typename T>
class state_allocator : public std::allocator<T> {
 public:
  using is_always_equal = std::false_type;
  using propagate_on_container_swap = std::true_type;
  using propagate_on_container_move_assignment = std::false_type;

  state_allocator() : std::allocator<T>(), state_("default"){}
  state_allocator(const std::string& state)
      : std::allocator<T>(), state_(state){}

  template <typename U>
  bool equals(const state_allocator<U>& other) const noexcept {
    return state_ == other.state_;
  }

  template <typename U>
  operator state_allocator<U>() {
    state_allocator<U> other(state_);
    return other;
  }

 private:
  std::string state_;
};

template <typename L, typename R>
bool operator==(const state_allocator<L>& lhs, const state_allocator<R>& rhs) {
  return lhs.equals(rhs);
}

template <typename L, typename R>
bool operator!=(const state_allocator<L>& lhs, const state_allocator<R>& rhs) {
  return !lhs.equals(rhs);
}
#endif  // SP_TESTS_TEST_HELPERS_H_
