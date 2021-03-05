
#ifndef _ZOO_SKUNK_NON_COPYABLE_H_
#define _ZOO_SKUNK_NON_COPYABLE_H_
namespace zoo {
namespace skunk {
class Noncopyable {
 public:
  Noncopyable(const Noncopyable& rhs) = delete;
  Noncopyable& operator=(const Noncopyable& rhs) = delete;

 protected:
  Noncopyable() = default;
  ~Noncopyable() = default;
};
}  // namespace skunk
}  // namespace zoo
#endif