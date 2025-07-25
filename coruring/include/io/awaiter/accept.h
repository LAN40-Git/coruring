#pragma once
#include "io/base/registrator.h"

namespace coruring::io {
namespace detail {
class Accept : public IoRegistrator<Accept> {
public:
    Accept(int fd, sockaddr* addr, socklen_t* addrlen, int flags)
        : IoRegistrator{io_uring_prep_accept, fd, addr, addrlen, flags} {}

    auto await_resume() noexcept -> std::expected<int, std::error_code> {
        if (this->cb_.result_ >= 0) [[likely]] {
            return this->cb_.result_;
        }
        return std::unexpected{std::error_code(-this->cb_.result_,
                                               std::generic_category())};
    }
};
} // namespace detail

[[REMEMBER_CO_AWAIT]]
static inline auto accept(int fd, sockaddr* addr, socklen_t* addrlen, int flags) {
    return detail::Accept{fd, addr, addrlen, flags};
}
} // namespace coruring::io