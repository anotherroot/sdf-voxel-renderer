#ifndef ARC_CORE
#define ARC_CORE
#include "log.h"
#include <memory>
namespace arc {
template <typename T> using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args &&...args) {
  return std::make_unique<T>(std::forward<Args>(args)...);
}
template <typename T> using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args &&...args) {
  return std::make_shared<T>(std::forward<Args>(args)...);
}

struct Size2D{
  uint32_t w, h;
};
struct Size3D{
  uint32_t w, h, d;
};

} // namespace arc
#endif
