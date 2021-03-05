#ifndef _ZOO_SKUNK_CALLBACKS_H_
#define _ZOO_SKUNK_CALLBACKS_H_
#include <functional>
#include <memory>

#include "connection.h"
namespace zoo {
namespace skunk {
class Connection;
/**
 * 各种用户自定义的回调函数
 */
using ConnectionCallback =
    std::function<void(const std::shared_ptr<Connection>)>;
using MessageCallback = std::function<void(const std::shared_ptr<Connection>)>;
using CloseCallback = std::function<void(const std::shared_ptr<Connection>)>;
using ErrorCallback = std::function<void(const std::shared_ptr<Connection>)>;
using TimerCallback = std::function<void()>;
};  // namespace skunk
}  // namespace zoo
#endif