#pragma once
#include"spinLock.h"

#include<queue>
template<typename T>
class messageQueue {
private:
    std::queue<T>m_queue;
    SpinLock m_lock;
public:
    messageQueue()noexcept;
    messageQueue(const messageQueue& other) = delete;
    messageQueue operator=(const messageQueue& other) = delete;
    messageQueue(const messageQueue&& other)noexcept;
    T front();
    T back();
    void pop();
    void push(T value);
    size_t size();
    bool empty();
    void clear();
};

template<typename T>
inline messageQueue<T>::messageQueue() noexcept { }

template<typename T>
inline messageQueue<T>::messageQueue(const messageQueue && other) noexcept {
    m_queue.swap(other.m_queue);
}

template<typename T>
inline T messageQueue<T>::front() {
    return m_queue.front();
}

template<typename T>
inline T messageQueue<T>::back() {
    return m_queue.back();
}

template<typename T>
inline void messageQueue<T>::pop() { 
    m_lock.lock();
    m_queue.pop();
    m_lock.unlock();
}

template<typename T>
inline void messageQueue<T>::push(T value) {
    m_lock.lock();
    m_queue.push(value);
    m_lock.unlock();
}

template<typename T>
inline size_t messageQueue<T>::size() {
    m_lock.lock();
    auto s=m_queue.size();
    m_lock.unlock();
    return s;
}

template<typename T>
inline bool messageQueue<T>::empty() {
    m_lock.lock();
    auto r = m_queue.empty();
    m_lock.unlock();
    return r;
}

template<typename T>
inline void messageQueue<T>::clear() { 
    m_lock.lock();
    while (!m_queue.empty()) {
        m_queue.pop();
    }
    m_lock.unlock();
}
