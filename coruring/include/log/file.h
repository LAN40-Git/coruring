#pragma once
#include <array>
#include <format>
#include <fstream>
#include <string>
#include <string_view>

namespace coruring::log::detail {
class LogFile {
public:
    explicit LogFile(std::string_view file_base_name)
        : file_base_name_(file_base_name) {
            this->roll();
            ofs_.rdbuf()->pubsetbuf(buffer_.data(), buffer_.size());
        }

    ~LogFile() {
        ofs_.close();
    }

    void write(const char *data, std::size_t len) {
        ofs_.write(data, len);
        cur_file_size_ += len;
        if (cur_file_size_ > max_file_size_) {
            roll();
        }
    }

    void flush() {
        ofs_.flush();
    }

    void set_max_file_size(off_t max_file_size) noexcept {
        max_file_size_ = max_file_size;
    }

private:
    void roll() {
        time_t current_second = ::time(nullptr);
        if (current_second > last_roll_second_) {
            tm tm_time{};
            ::localtime_r(&current_second, &tm_time);

            last_roll_day_ = current_second / SECONDS_PER_DAY;
            last_roll_second_ = current_second;
            cur_file_size_ = 0;
            ofs_.close();

            std::string file_name{file_base_name_};
            file_name.reserve(file_name.size());
            std::array<char, 64> buf{};
            ::strftime(buf.data(), buf.size(), "-%Y%m%d-%H%M%S.log", &tm_time);
            file_name.append(buf.data());
            ofs_.open(file_name, std::ios::out);
            if (!ofs_.is_open()) [[unlikely]] {
                throw std::runtime_error(std::format("open file {} failed", file_name));
            }
        }
    }

private:
    constexpr static std::size_t SECONDS_PER_DAY{60 * 60 * 24};
    constexpr static std::size_t BUFFER_SIZE{64 * 1024};

private:
    std::ofstream                 ofs_{};
    const std::string             file_base_name_;
    off_t                         max_file_size_{10 * 1024 * 1024};
    off_t                         cur_file_size_{0};
    std::array<char, BUFFER_SIZE> buffer_;
    time_t                        last_roll_day_{0};
    time_t                        last_roll_second_{0};
};
} // namespace coruring::log
