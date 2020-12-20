//
// Created by jian.yeung on 2020/12/7.
//
#include <DLog.h>
#include "Looper.h"

#include "Handler.h"

void Looper::run(Looper *looper) {
    auto my_looper_ = looper;
    while (true) {
        Message msg;
        {
            std::unique_lock<std::mutex> lock(my_looper_->queue_mutex_);
            if (my_looper_->msg_queue_.empty()) {
                my_looper_->queue_condition_.wait(lock, [my_looper_] {
                    return my_looper_->stop || my_looper_->stopSafety || !my_looper_->msg_queue_.empty();
                });
            } else {
                my_looper_->queue_condition_.wait_until(lock, my_looper_->msg_queue_.back().when, [my_looper_]  {
                    return my_looper_->stop || my_looper_->stopSafety || !my_looper_->msg_queue_.empty();
                });
            }

            if (my_looper_->stopSafety && my_looper_->msg_queue_.empty()) {
                return;
            }

            if (my_looper_->stop) {
                my_looper_->msg_queue_.clear();
                return;
            }

            if (!my_looper_->msg_queue_.empty()) {
                msg = std::move(my_looper_->msg_queue_.back());
                my_looper_->msg_queue_.pop_back();
            }
        }

        if (msg.target != nullptr) {
            msg.target->dispatchMessage(msg);
        }
    }
}

Looper::Looper() {
    DLOGI("Looper", "~~~Looper::Looper~~~\n");
    msg_queue_ = std::vector<Message>();
    msg_thread_ = std::thread(&Looper::run, this);
}

Looper::~Looper() {
    DLOGI("Looper", "~~~Looper::~Looper~~~\n");
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        stop = true;
    }
    queue_condition_.notify_all();
    msg_thread_.join();
    msg_queue_.clear();
}

bool Looper::enqueueMessage(Message &msg) {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    auto i = std::find(msg_queue_.begin(), msg_queue_.end(), msg);
    if (i != msg_queue_.end()) {
        msg_queue_.erase(i);
    }

    if (msg.target == nullptr && msg.task == nullptr) {
        return false;
    }

    msg_queue_.push_back(msg);
    std::sort(msg_queue_.begin(), msg_queue_.end(), greater<Message>());
    queue_condition_.notify_one();
    return true;
}

void Looper::dequeueMessage(int what) {
    if (what < 0) {
        return;
    }
    std::unique_lock<std::mutex> lock(queue_mutex_);
    auto i = std::find(msg_queue_.begin(), msg_queue_.end(), what);
    if (i != msg_queue_.end()) {
        msg_queue_.erase(i);
    }
}

void Looper::dequeueAllMessage() {
    std::unique_lock<std::mutex> lock(queue_mutex_);
    msg_queue_.clear();
}

void Looper::quit() {
    stop = true;
}

void Looper::quitSafety() {
    stopSafety = true;
}


