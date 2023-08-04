#ifndef DLWHI_TESTS_TEST_HELPERS_H_
#define DLWHI_TESTS_TEST_HELPERS_H_
#include <string>
#include <memory>

struct IntWrapper {
  int data;
};

// CopyConstructible and Erasable dummy
class dummyCpy
{
  public:
    dummyCpy() : id_("default"), leak_(new int()) { };
    explicit dummyCpy(const std::string& name) : id_(name), leak_(new int()) { };
    dummyCpy(dummyCpy&& other) = delete;
    dummyCpy(const dummyCpy& other) : id_(other.id_), leak_(new int()) { };
    virtual ~dummyCpy() { delete leak_;}

    bool operator==(const dummyCpy& other) const { return other.id_ == id_;}
    bool operator!=(const dummyCpy& other) const { return other.id_ != id_;}

    friend std::ostream& operator<<(std::ostream& os, const dummyCpy& obj) {
      os << obj.id_;
      return os;
    }
  protected:
    std::string id_;
    int* leak_;
};

// + MoveConstructible dummy
class dummyMv : public dummyCpy
{
  public:
    enum class Constructed {
      kDef,
      kParam,
      kCopy,
      kMove
    };

    dummyMv() : dummyCpy(), birth(Constructed::kDef) { };
    explicit dummyMv(const std::string& name) : dummyCpy(name), birth(Constructed::kParam) { };
    dummyMv(const dummyMv& other) : dummyCpy(other.id_), birth(Constructed::kCopy) { };
    // speed dies as this is test class
    dummyMv(dummyMv&& other): birth(Constructed::kMove) { 
      id_ = std::move(other.id_);
      delete leak_;
      leak_ = other.leak_;
      other.leak_ = nullptr;
    }
    virtual ~dummyMv() { }

    const Constructed birth;
};

// CopyConstructible and Erasable dummy
class dummyNoDef
{
  public:
    explicit dummyNoDef(const std::string& name) : id_(name), leak_(new int()) { };
    dummyNoDef(dummyNoDef&& other) = delete;
    dummyNoDef(const dummyNoDef& other) : id_(other.id_), leak_(new int()) { };
    virtual ~dummyNoDef() { delete leak_;}

    bool operator==(const dummyNoDef& other) const { return other.id_ == id_;}
    bool operator!=(const dummyNoDef& other) const { return other.id_ != id_;}

    friend std::ostream& operator<<(std::ostream& os, const dummyNoDef& obj) {
      os << obj.id_;
      return os;
    }
  protected:
    std::string id_;
    int* leak_;
};

// state allocator
template <typename T>
class state_allocator : public std::allocator<T> {
  public:
    typedef std::false_type is_always_equal;

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
