#pragma once
#include "io/base/registrator.h"

namespace coruring::io {
namespace detail {
class Read : public IoRegistrator<Read> {
public:
    Read(int fd, void* buf, unsigned nbytes, __u64 offse)
        : IoRegistrator{io_uring_prep_read, fd, buf, nbytes, offse} {}

    auto await_resume() noexcept -> std::expected<std::size_t, std::error_code> {
        if (this->cb_.result_ >= 0) [[likely]] {
            return static_cast<std::size_t>(this->cb_.result_);
        }
        return std::unexpected{std::error_code(-this->cb_.result_,
                                               std::generic_category())};
    }
};
} // namespace detail

[[REMEMBER_CO_AWAIT]]
static inline auto read(int fd, void* buf, unsigned nbytes, __u64 offse) {
    return detail::Read{fd, buf, nbytes, offse};
}
} // namespace coruring::io