#include "runtime/timer/entry.h"

void coruring::runtime::detail::Entry::execute() {
    // 操作完成时，data_ 会被设置为 nullptr
    // 若操作未完成则提交取消请求
    if (data_) {
        if (auto sqe = t_ring->get_sqe()) [[likely]] {
            io_uring_prep_cancel(sqe, data_, 0);
            io_uring_sqe_set_data(sqe, nullptr);
            t_ring->pend_submit();
        }
        data_->entry_ = nullptr;
    }
    // 操作完成，不需要进行操作（事件已被弹出分层事件轮）
}
