#ifndef SP_TESTS_STANDART_TEST_HELPERS_H_
#define SP_TESTS_STANDART_TEST_HELPERS_H_

#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>

enum class constructed { kDef, kParam, kCopy, kMove };
enum traits {
  NoTraits = 0x00,
  DefaultConsrtuctible = 0x01,
  DefaultInsertable = 0x01,
  MoveAssignable = 0x02,
  MoveConsructible = 0x04,
  MoveInsertable = 0x04,
  CopyAssignable = 0x10,
  CopyConsructible = 0x20,
  CopyInsertable = 0x20,
  Full = 0xFF
};

constexpr traits operator|(traits lhs, traits rhs) noexcept {
    return static_cast<traits>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

template <traits signature>
class tclass {
 public:
  tclass()
    requires((signature & DefaultConsrtuctible) == DefaultConsrtuctible)
      : birth(constructed::kDef), id_("default"), leak_(new int()) {}

  explicit tclass(const std::string& name)
      : birth(constructed::kParam), id_(name), leak_(new int()) {}

  tclass(tclass&& other) noexcept
    requires((signature & MoveConsructible) == MoveConsructible)
      : birth(constructed::kMove) {
    id_ = std::move(other.id_);
    leak_ = other.leak_;
    other.leak_ = nullptr;
  }

  tclass(const tclass& other)
    requires((signature & CopyConsructible) == CopyConsructible)
      : birth(constructed::kCopy), id_(other.id_), leak_(new int()) {}

  tclass& operator=(const tclass& other) requires((signature & CopyAssignable) == CopyAssignable) {
    id_ = other.id_;
    return *this;
  }

  tclass& operator=(tclass&& other) noexcept requires((signature & MoveAssignable) == MoveAssignable) {
    id_ = std::move(other.id_);
    return *this;
  }

  virtual ~tclass() noexcept { delete leak_; }

  bool operator==(const tclass& other) const { return other.id_ == id_; }
  bool operator!=(const tclass& other) const { return other.id_ != id_; }

  friend std::ostream& operator<<(std::ostream& os, const tclass& obj) {
    os << obj.id_;
    return os;
  }
  const constructed birth;
 protected:
  std::string id_;
  int* leak_;
};

// exception throwing dummy
class throwing : public tclass<Full> {
 public:
  throwing() : tclass() {}
  explicit throwing(const std::string& name) : tclass(name) {}
  throwing(throwing&& other) : tclass(std::move(other)) {
    if (!(++count % 5)) {
      throw std::runtime_error("IN YOUR FUCKING FACE");
    }
    id_ = std::move(other.id_);
  };
  throwing(const throwing& other) : tclass(other) {
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

#endif  // SP_TESTS_STANDART_TEST_HELPERS_H_
