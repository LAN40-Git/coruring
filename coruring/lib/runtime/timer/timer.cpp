#include "runtime/timer/timer.h"

auto coruring::runtime::Timer::add_entry(io::detail::Callback* data,
    uint64_t expiration_time) noexcept -> std::expected<detail::Entry*, std::error_code> {
    if (expiration_time <= start_) [[unlikely]] {
        return std::unexpected{std::error_code{static_cast<int>(std::errc::invalid_argument),
             std::system_category()}};
    }
    auto remaining_ms = expiration_time - start_;
    if (remaining_ms >= detail::Wheel::MAX_DURATION) [[unlikely]] {
            return std::unexpected{std::error_code{static_cast<int>(std::errc::invalid_argument),
                 std::system_category()}};
    }
    auto entry = std::make_unique<detail::Entry>(detail::Entry{data, expiration_time});
    return wheel_.add_entry(std::move(entry), remaining_ms);
}

void coruring::runtime::Timer::remove_entry(detail::Entry* entry) noexcept {
    if (entry->expiration_time_ <= start_) [[unlikely]] {
        return;
    }

    wheel_.remove_entry(entry, entry->expiration_time_ - start_);
}
