#ifndef DLWHI_TESTS_TEST_HELPERS_H_
#define DLWHI_TESTS_TEST_HELPERS_H_
#include <string>
#include <memory>
// This is testing subject class with restrictions
// Several implementations satisfy different named requirements
// Vector requires object to be at least
// CopyAssignable, CopyConstructible and Erasable
// Some methods could alse require MoveAssignable and MoveConstructible

// CopyAssignable, CopyConstructible and Erasable dummy
class dummy
{
  public:
    dummy() : id_("default"), leak_(new int()) { };
    dummy(const std::string& name) : id_(name), leak_(new int()) { };
    dummy(dummy&& other) = delete;
    dummy(const dummy& other) : id_(other.id_), leak_(new int()) { };
    dummy& operator=(const dummy& other) {
      id_ = other.id_; 
      return *this;
    }
    virtual ~dummy() { delete leak_;}

    bool operator==(const dummy& other) const { return other.id_ == id_;}
    bool operator!=(const dummy& other) const { return other.id_ != id_;}

    friend std::ostream& operator<<(std::ostream& os, const dummy& obj) {
      os << obj.id_;
      return os;
    }
  protected:
    std::string id_;
    int* leak_;
};

// + MoveAssignable and MoveConstructible dummy
class dummyMovable : public dummy
{
  public:
    dummyMovable() : dummy() { };
    dummyMovable(const std::string& name) : dummy(name) { };
    dummyMovable(const dummyMovable& other) : dummy(other.id_) { };
    // speed dies as this is test class
    dummyMovable(dummyMovable&& other) { 
      id_ = std::move(other.id_);
      delete leak_;
      leak_ = other.leak_;
      other.leak_ = nullptr;
    }
    dummyMovable& operator=(dummyMovable&& other) { std::swap(leak_, other.leak_); id_ = std::move(other.id_); return *this;}
    dummyMovable& operator=(const dummyMovable& other) { dummy::operator=(other); return *this;}

    virtual ~dummyMovable() { }
};

template <typename T>
class state_allocator : public std::allocator<T> {
  public:
    state_allocator(const std::string& state) : std::allocator<T>(), state_(state) {};

    bool operator==(const state_allocator& other) {
      return state_ == other.state_;
    }
    bool operator!=(const state_allocator& other) {
      return state_ != other.state_;
    }
  private:
    std::string state_;
};
#endif  // DLWHI_TESTS_TEST_HELPERS_H_
