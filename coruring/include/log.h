#pragma once
#include "log/logger.h"
#include "common/util/singleton.h"

namespace coruring::log {
inline auto& console = util::Singleton<ConsoleLogger>::instance();
} // namespace coruring::log
