#ifndef THREADSAFE_QUEUE_H
#define THREADSAFE_QUEUE_H
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

template <typename T> struct queueNode {
    std::shared_ptr<T>         data;
    std::unique_ptr<queueNode> next;
};

template <typename T> class threadsafeQueue {
private:
    std::mutex                    headmu;
    std::mutex                    tailmu;
    std::unique_ptr<queueNode<T>> head;
    queueNode<T>*                 tail;
    std::condition_variable       cv;
    queueNode<T>*                 getTail();
    std::unique_ptr<queueNode<T>> popHead();
    std::unique_lock<std::mutex>  waitForData();
    std::unique_ptr<queueNode<T>> waitPopHead();
    std::unique_ptr<queueNode<T>> waitPopHead(T& value);
    std::unique_ptr<queueNode<T>> tryPopHead();
    std::unique_ptr<queueNode<T>> tryPopHead(T& value);

public:
    threadsafeQueue() : head(new queueNode<T>), tail(head.get()) {}
    threadsafeQueue(const threadsafeQueue& other) = delete;
    threadsafeQueue& operator=(const threadsafeQueue& other) = delete;

    std::shared_ptr<T> tryPop();
    bool               tryPop(T& value);
    std::shared_ptr<T> waitAndPop();
    void               waitAndPop(T& value);
    void               push(T newValue);
    void               empty();
    void               clear();
};

template <typename T>
std::unique_ptr<queueNode<T>> threadsafeQueue<T>::tryPopHead() {
    std::lock_guard<std::mutex> headLock(headmu);
    if (head.get() == getTail()) {
        return std::unique_ptr<queueNode<T>>();
    }
    return popHead();
}

template <typename T>
std::unique_ptr<queueNode<T>> threadsafeQueue<T>::tryPopHead(T& value) {
    std::lock_guard<std::mutex> headLock(headmu);
    if (head.get() == getTail()) {
        return std::unique_ptr<queueNode<T>>();
    }
    value = std::move(*head->data);
    return popHead();
}

template <typename T> queueNode<T>* threadsafeQueue<T>::getTail() {
    std::lock_guard<std::mutex> tailLock(tailmu);
    return tail;
}

template <typename T>
std::unique_ptr<queueNode<T>> threadsafeQueue<T>::popHead() {
    std::unique_ptr<queueNode<T>> oldHead = std::move(head);
    head                                  = std::move(oldHead->next);
    return oldHead;
}

template <typename T>
std::unique_lock<std::mutex> threadsafeQueue<T>::waitForData() {
    std::unique_lock<std::mutex> headLock(headmu);
    cv.wait(headLock, [&] { return head.get() != getTail(); });
    return std::move(headLock);
}

template <typename T>
std::unique_ptr<queueNode<T>> threadsafeQueue<T>::waitPopHead() {
    std::unique_lock<std::mutex> headLock(waitForData());
    return popHead();
}

template <typename T>
std::unique_ptr<queueNode<T>> threadsafeQueue<T>::waitPopHead(T& value) {
    std::unique_lock<std::mutex> headLock(waitForData());
    value = std::move(*head->data);
    return popHead();
}

template <typename T> void threadsafeQueue<T>::push(T newValue) {
    std::shared_ptr<T> newData(std::make_shared<T>(std::move(newValue)));
    std::unique_ptr<queueNode<T>> p(new queueNode<T>);
    {
        std::lock_guard<std::mutex> taillock(tailmu);
        tail->data                  = newData;
        queueNode<T>* const newTail = p.get();
        tail->next                  = std::move(p);
        tail                        = newTail;
    }
    cv.notify_one();
}

template <typename T> std::shared_ptr<T> threadsafeQueue<T>::waitAndPop() {
    std::unique_ptr<queueNode<T>> const oldHead = waitPopHead();
    return oldHead->data;
}

template <typename T> void threadsafeQueue<T>::waitAndPop(T& value) {
    std::unique_ptr<queueNode<T>> const oldHead = waitPopHead(value);
}

template <typename T> std::shared_ptr<T> threadsafeQueue<T>::tryPop() {
    std::unique_ptr<queueNode<T>> oldHead = tryPopHead();
    return oldHead ? oldHead->data : std::shared_ptr<T>();
}

template <typename T> bool threadsafeQueue<T>::tryPop(T& value) {
    std::unique_ptr<queueNode<T>> const oldHead = tryPopHead(value);
    return oldHead;
}

template <typename T> void threadsafeQueue<T>::empty() {
    std::lock_guard<std::mutex> headLock(headmu);
    return (head.get() == getTail());
}

template <typename T> void threadsafeQueue<T>::clear() {
    while (tryPopHead()) {
    }
}

#endif  // THREADSAFE_QUEUE_H
