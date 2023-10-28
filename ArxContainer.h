#pragma once

#ifndef ARX_RINGBUFFER_H
#define ARX_RINGBUFFER_H

#include "ArxContainer/has_include.h"
#include "ArxContainer/has_libstdcplusplus.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include "ArxContainer/replace_minmax_macros.h"
#include "ArxContainer/initializer_list.h"

#if ARX_HAVE_LIBSTDCPLUSPLUS >= 201103L  // Have libstdc++11

#include <vector>
#include <array>
#include <deque>
#include <map>

#else  // Do not have libstdc++11

#include <limits.h>

#ifndef ARX_VECTOR_DEFAULT_SIZE
#define ARX_VECTOR_DEFAULT_SIZE 16
#endif  // ARX_VECTOR_DEFAULT_SIZE

#ifndef ARX_DEQUE_DEFAULT_SIZE
#define ARX_DEQUE_DEFAULT_SIZE 16
#endif  // ARX_DEQUE_DEFAULT_SIZE

#ifndef ARX_MAP_DEFAULT_SIZE
#define ARX_MAP_DEFAULT_SIZE 16
#endif  // ARX_MAP_DEFAULT_SIZE

namespace arx {

namespace container {
    namespace detail {
        template <class T>
        T&& move(T& t) { return static_cast<T&&>(t); }
    }  // namespace detail
}  // namespace container

template <typename T, size_t N>
class RingBuffer {
    class Iterator;
    class ConstIterator {
        friend RingBuffer<T, N>;

        const T* ptr {nullptr};  // pointer to the first element
        int pos {0};

        ConstIterator(const T* ptr, int pos)
        : ptr(ptr), pos(pos) {}

    public:
        ConstIterator() {}
        ConstIterator(const ConstIterator& it) {
            this->ptr = it.ptr;
            this->pos = it.pos;
        }

        ConstIterator(ConstIterator&& it) {
            this->ptr = container::detail::move(it.ptr);
            this->pos = container::detail::move(it.pos);
        }

        ConstIterator& operator=(const ConstIterator& rhs) {
            this->ptr = rhs.ptr;
            this->pos = rhs.pos;
            return *this;
        }
        ConstIterator& operator=(ConstIterator&& rhs) {
            this->ptr = container::detail::move(rhs.ptr);
            this->pos = container::detail::move(rhs.pos);
            return *this;
        }

        // const-like conversion ConstIterator => Iterator
        Iterator to_iterator() const {
            return Iterator(this->ptr, this->pos);
        }

    private:
        static int pos_wrap_around(const int pos) {
            if (pos >= 0)
                return pos % N;
            else
                return (N - 1) - (abs(pos + 1) % N);
        }

    public:
        int index() const {
            return pos_wrap_around(pos);
        }

        int index_with_offset(const int i) const {
            return pos_wrap_around(pos + i);
        }

        const T& operator*() const {
            return *(ptr + index());
        }
        const T* operator->() const {
            return ptr + index();
        }

        ConstIterator operator+(const int n) const {
            return ConstIterator(this->ptr, this->pos + n);
        }
        int operator-(const ConstIterator& rhs) const {
            return this->pos - rhs.pos;
        }
        ConstIterator operator-(const int n) const {
            return ConstIterator(this->ptr, this->pos - n);
        }
        ConstIterator& operator+=(const int n) {
            this->pos += n;
            return *this;
        }
        ConstIterator& operator-=(const int n) {
            this->pos -= n;
            return *this;
        }

        // prefix increment/decrement
        ConstIterator& operator++() {
            ++pos;
            return *this;
        }
        ConstIterator& operator--() {
            --pos;
            return *this;
        }
        // postfix increment/decrement
        ConstIterator operator++(int) {
            ConstIterator it = *this;
            ++pos;
            return it;
        }
        ConstIterator operator--(int) {
            ConstIterator it = *this;
            --pos;
            return it;
        }

        bool operator==(const ConstIterator& rhs) const {
            return (rhs.ptr == ptr) && (rhs.pos == pos);
        }
        bool operator!=(const ConstIterator& rhs) const {
            return !(*this == rhs);
        }
        bool operator<(const ConstIterator& rhs) const {
            return pos < rhs.pos;
        }
        bool operator<=(const ConstIterator& rhs) const {
            return pos <= rhs.pos;
        }
        bool operator>(const ConstIterator& rhs) const {
            return pos > rhs.pos;
        }
        bool operator>=(const ConstIterator& rhs) const {
            return pos >= rhs.pos;
        }

    private:
        int raw_pos() const {
            return pos;
        }

        void set(const int i) {
            pos = i;
        }

        void reset() {
            pos = 0;
        }
    };

    class Iterator : public ConstIterator {
        friend RingBuffer<T, N>;

        Iterator(const T* ptr, int pos) {
            this->ptr = ptr;
            this->pos = pos;
        }

    public:
        Iterator() = default;
        Iterator(const Iterator&) = default;
        Iterator(Iterator&&) = default;
        Iterator& operator=(const Iterator&) = default;
        Iterator& operator=(Iterator&&) = default;

        T& operator*() {
            return *(const_cast<T*>(this->ptr) + this->index());
        }
        T* operator->() {
            return const_cast<T*>(this->ptr) + this->index();
        }

        // all inherited methods that return ConstIterator must be reimplemented
        Iterator operator+(const int n) const {
            return Iterator(this->ptr, this->pos + n);
        }
        Iterator operator-(const int n) const {
            return Iterator(this->ptr, this->pos - n);
        }
        Iterator& operator+=(const int n) {
            this->pos += n;
            return *this;
        }
        Iterator& operator-=(const int n) {
            this->pos -= n;
            return *this;
        }

        // prefix increment/decrement
        Iterator& operator++() {
            ++(this->pos);
            return *this;
        }
        Iterator& operator--() {
            --(this->pos);
            return *this;
        }
        // postfix increment/decrement
        Iterator operator++(int) {
            Iterator it = *this;
            ++(this->pos);
            return it;
        }
        Iterator operator--(int) {
            Iterator it = *this;
            --(this->pos);
            return it;
        }
    };

protected:
    friend class Iterator;
    friend class ConstIterator;

    T queue_[N];
    Iterator head_;
    Iterator tail_;

public:
    using iterator = Iterator;
    using const_iterator = ConstIterator;

    RingBuffer()
    : queue_()
    , head_(queue_, 0)
    , tail_(queue_, 0) {
    }

    RingBuffer(std::initializer_list<T> lst)
    : queue_()
    , head_(queue_, 0)
    , tail_(queue_, 0) {
        for (auto it = lst.begin(); it != lst.end(); ++it) {
            push_back(*it);
        }
    }

    // copy
    explicit RingBuffer(const RingBuffer& r)
    : queue_()
    , head_(queue_, r.head_.raw_pos())
    , tail_(queue_, r.tail_.raw_pos()) {
        for (size_t i = 0; i < r.size(); ++i) {
            int pos = ConstIterator::pos_wrap_around(r.head_.raw_pos() + (int)i);
            queue_[pos] = r.queue_[pos];
        }
    }
    RingBuffer& operator=(const RingBuffer& r) {
        head_.set(r.head_.raw_pos());
        tail_.set(r.tail_.raw_pos());
        for (size_t i = 0; i < r.size(); ++i) {
            int pos = ConstIterator::pos_wrap_around(r.head_.raw_pos() + (int)i);
            queue_[pos] = r.queue_[pos];
        }
        return *this;
    }

    // move
    RingBuffer(RingBuffer&& r) {
        head_ = container::detail::move(r.head_);
        tail_ = container::detail::move(r.tail_);
        for (size_t i = 0; i < r.size(); ++i) {
            int pos = ConstIterator::pos_wrap_around(r.head_.raw_pos() + (int)i);
            queue_[pos] = container::detail::move(r.queue_[pos]);
        }
    }

    RingBuffer& operator=(RingBuffer&& r) {
        head_ = container::detail::move(r.head_);
        tail_ = container::detail::move(r.tail_);
        for (size_t i = 0; i < r.size(); ++i) {
            int pos = ConstIterator::pos_wrap_around(r.head_.raw_pos() + (int)i);
            queue_[pos] = container::detail::move(r.queue_[pos]);
        }
        return *this;
    }

    virtual ~RingBuffer() {}

    size_t capacity() const { return N; };
    size_t size() const { return abs(tail_.raw_pos() - head_.raw_pos()); }
    inline const T* data() const { return &(get(head_)); }
    T* data() { return &(get(head_)); }
    bool empty() const { return tail_ == head_; }
    void clear() {
        head_.reset();
        tail_.reset();
    }

    void pop() {
        pop_front();
    }
    void pop_front() {
        if (size() == 0) return;
        if (size() == 1)
            clear();
        else
            increment_head();
    }
    void pop_back() {
        if (size() == 0) return;
        if (size() == 1)
            clear();
        else
            decrement_tail();
    }

    void push(const T& data) {
        push_back(data);
    }
    void push(T&& data) {
        push_back(data);
    }
    void push_back(const T& data) {
        get(tail_) = data;
        increment_tail();
    };
    void push_back(T&& data) {
        get(tail_) = data;
        increment_tail();
    };
    void push_front(const T& data) {
        decrement_head();
        get(head_) = data;
    };
    void push_front(T&& data) {
        decrement_head();
        get(head_) = data;
    };
    void emplace(const T& data) { push(data); }
    void emplace(T&& data) { push(data); }
    void emplace_back(const T& data) { push_back(data); }
    void emplace_back(T&& data) { push_back(data); }

    const T& front() const { return get(head_); }
    T& front() { return get(head_); }

    const T& back() const { return get(size() - 1); }
    T& back() { return get(size() - 1); }

    const T& operator[](size_t index) const { return get((int)index); }
    T& operator[](size_t index) { return get((int)index); }

    iterator begin() { return empty() ? Iterator() : head_; }
    iterator end() { return empty() ? Iterator() : tail_; }
    const_iterator begin() const { return empty() ? ConstIterator() : static_cast<ConstIterator>(head_); }
    const_iterator end() const { return empty() ? ConstIterator() : static_cast<ConstIterator>(tail_); }

    // https://en.cppreference.com/w/cpp/container/vector/erase
    iterator erase(const const_iterator& p) {
        if (!is_valid(p)) return end();

        iterator it_last = end() - 1;
        for (iterator it = p.to_iterator(); it != it_last; ++it)
            *it = *(it + 1);
        *it_last = T();
        decrement_tail();
        return empty() ? end() : p.to_iterator();
    }

    void resize(size_t sz) {
        size_t s = size();
        if (sz > size()) {
            for (size_t i = 0; i < sz - s; ++i) push(T());
        } else if (sz < size()) {
            for (size_t i = 0; i < s - sz; ++i) pop();
        }
    }

    void assign(const_iterator first, const_iterator end) {
        clear();
        while (first != end) push(*(first++));
    }

    void assign(const T* first, const T* end) {
        clear();
        // T* it = first;
        while (first != end) push(*(first++));
    }

    void shrink_to_fit() {
        // dummy
    }

    void reserve(size_t n) {
        (void)n;
        // dummy
    }

    void fill(const T& v) {
        iterator it = begin();
        while (it != end()) {
            *it = v;
            ++it;
        }
    }

    // https://en.cppreference.com/w/cpp/container/vector/insert
    void insert(const const_iterator& pos, const const_iterator& first, const const_iterator& last) {
        if (!is_valid(pos) && pos != end())
            return;

        size_t sz = last - first;

        size_t new_sz = size() + sz;
        if (new_sz > capacity())
            new_sz = capacity();

        iterator it = begin() + new_sz - 1;
        while (it != pos) {
            *it = *(it - sz);
            --it;
        }
        for (size_t i = 0; i < sz; ++i) {
            *(it + i) = *(first + i);
            if (size() < capacity() || (it + i) == end())
                increment_tail();
        }
    }

    void insert(const const_iterator& pos, const T* first, const T* last) {
        if (!is_valid(pos) && pos != end())
            return;

        size_t sz = last - first;

        size_t new_sz = size() + sz;
        if (new_sz > capacity())
            new_sz = capacity();
            
        iterator it = begin() + new_sz - 1;
        while (it != pos) {
            *it = *(it - sz);
            --it;
        }
        for (size_t i = 0; i < sz; ++i) {
            *(it + i) = *(first + i);
            if (size() < capacity() || (it + i) == end())
                increment_tail();
        }
    }

    void insert(const const_iterator& pos, const T& val) {
        const T* ptr = &val;
        insert(pos, ptr, ptr + 1);
    }

private:
    T& get(const iterator& it) {
        return queue_[it.index()];
    }
    const T& get(const const_iterator& it) const {
        return queue_[it.index()];
    }
    T& get(const int index) {
        return queue_[head_.index_with_offset(index)];
    }
    const T& get(const int index) const {
        return queue_[head_.index_with_offset(index)];
    }

    T* ptr(const iterator& it) {
        return queue_ + it.index();
    }
    const T* ptr(const const_iterator& it) const {
        return queue_ + it.index();
    }
    T* ptr(const int index) {
        return queue_ + head_.index_with_offset(index);
    }
    const T* ptr(const int index) const {
        return queue_ + head_.index_with_offset(index);
    }

    void increment_head() {
        ++head_;
        resolve_overflow();
    }
    void increment_tail() {
        ++tail_;
        resolve_overflow();
        if (size() > N)
            increment_head();
    }
    void decrement_head() {
        --head_;
        resolve_overflow();
        if (size() > N)
            decrement_tail();
    }
    void decrement_tail() {
        --tail_;
        resolve_overflow();
    }

    void resolve_overflow() {
        if (empty())
            clear();
        else if (head_.raw_pos() <= ((int)INT_MIN + (int)capacity()) \
                || tail_.raw_pos() >= ((int)INT_MAX - (int)capacity())) {
            // +/- capacity(): reserve some space for pointer/iterator arithmetics
            // pointer are newly set N+1 steps before the overflow occurs
            int len = size();
            head_.set(begin().index());
            tail_.set(head_.raw_pos() + len);
        }
    }

    bool is_valid(const const_iterator& it) const {
        if (it.ptr != queue_)
            return false; // iterator to a different object
        return (it.raw_pos() >= head_.raw_pos()) && (it.raw_pos() < tail_.raw_pos());
    }
};

template <typename T, size_t N>
bool operator==(const RingBuffer<T, N>& x, const RingBuffer<T, N>& y) {
    if (x.size() != y.size()) return false;
    for (size_t i = 0; i < x.size(); ++i)
        if (x[i] != y[i]) return false;
    return true;
}

template <typename T, size_t N>
bool operator!=(const RingBuffer<T, N>& x, const RingBuffer<T, N>& y) {
    return !(x == y);
}

template <typename T, size_t N = ARX_VECTOR_DEFAULT_SIZE>
struct vector : public RingBuffer<T, N> {
    using iterator = typename RingBuffer<T, N>::iterator;
    using const_iterator = typename RingBuffer<T, N>::const_iterator;

    vector()
    : RingBuffer<T, N>() {}
    vector(std::initializer_list<T> lst)
    : RingBuffer<T, N>(lst) {}

    // copy
    vector(const vector& r)
    : RingBuffer<T, N>(r) {}

    vector& operator=(const vector& r) {
        RingBuffer<T, N>::operator=(r);
        return *this;
    }

    // move
    vector(vector&& r)
    : RingBuffer<T, N>(r) {}

    vector& operator=(vector&& r) {
        RingBuffer<T, N>::operator=(r);
        return *this;
    }

    virtual ~vector() {}

private:
    using RingBuffer<T, N>::pop;
    using RingBuffer<T, N>::pop_front;
    using RingBuffer<T, N>::push;
    using RingBuffer<T, N>::push_front;
    using RingBuffer<T, N>::emplace;
    using RingBuffer<T, N>::fill;
};

template <typename T, size_t N>
struct array : public RingBuffer<T, N> {
    using iterator = typename RingBuffer<T, N>::iterator;
    using const_iterator = typename RingBuffer<T, N>::const_iterator;

    array()
    : RingBuffer<T, N>() {}
    array(std::initializer_list<T> lst)
    : RingBuffer<T, N>(lst) {}

    // copy
    array(const array& r)
    : RingBuffer<T, N>(r) {}

    array& operator=(const array& r) {
        RingBuffer<T, N>::operator=(r);
        return *this;
    }

    // move
    array(array&& r)
    : RingBuffer<T, N>(r) {}

    array& operator=(array&& r) {
        RingBuffer<T, N>::operator=(r);
        return *this;
    }

    virtual ~array() {}

private:
    using RingBuffer<T, N>::pop;
    using RingBuffer<T, N>::pop_front;
    using RingBuffer<T, N>::push;
    using RingBuffer<T, N>::push_front;
    using RingBuffer<T, N>::emplace;
};

template <typename T, size_t N = ARX_DEQUE_DEFAULT_SIZE>
struct deque : public RingBuffer<T, N> {
    using iterator = typename RingBuffer<T, N>::iterator;
    using const_iterator = typename RingBuffer<T, N>::const_iterator;

    deque()
    : RingBuffer<T, N>() {}
    deque(std::initializer_list<T> lst)
    : RingBuffer<T, N>(lst) {}

    // copy
    deque(const deque& r)
    : RingBuffer<T, N>(r) {}

    deque& operator=(const deque& r) {
        RingBuffer<T, N>::operator=(r);
        return *this;
    }

    // move
    deque(deque&& r)
    : RingBuffer<T, N>(r) {}

    deque& operator=(deque&& r) {
        RingBuffer<T, N>::operator=(r);
        return *this;
    }

    virtual ~deque() {}

private:
    using RingBuffer<T, N>::capacity;
    using RingBuffer<T, N>::pop;
    using RingBuffer<T, N>::push;
    using RingBuffer<T, N>::emplace;
    using RingBuffer<T, N>::assign;
    using RingBuffer<T, N>::begin;
    using RingBuffer<T, N>::end;
    using RingBuffer<T, N>::fill;
};

template <class T1, class T2>
struct pair {
    T1 first;
    T2 second;
};

template <class T1, class T2>
pair<T1, T2> make_pair(const T1& t1, const T2& t2) {
    return {t1, t2};
};

template <typename T1, typename T2>
bool operator==(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return (x.first == y.first) && (x.second == y.second);
}

template <typename T1, typename T2>
bool operator!=(const pair<T1, T2>& x, const pair<T1, T2>& y) {
    return !(x == y);
}

template <class Key, class T, size_t N = ARX_MAP_DEFAULT_SIZE>
struct map : public RingBuffer<pair<Key, T>, N> {
    using base = RingBuffer<pair<Key, T>, N>;
    using iterator = typename base::iterator;
    using const_iterator = typename base::const_iterator;

    map()
    : base() {}
    map(std::initializer_list<pair<Key, T> > lst)
    : base(lst) {}

    // copy
    map(const map& r)
    : base(r) {}

    map& operator=(const map& r) {
        base::operator=(r);
        return *this;
    }

    // move
    map(map&& r)
    : base(r) {}

    map& operator=(map&& r) {
        base::operator=(r);
        return *this;
    }

    virtual ~map() {}

    const_iterator find(const Key& key) const {
        for (size_t i = 0; i < this->size(); ++i) {
            const_iterator it = this->begin() + i;
            if (key == it->first)
                return it;
        }
        return this->end();
    }

    iterator find(const Key& key) {
        for (size_t i = 0; i < this->size(); ++i) {
            iterator it = this->begin() + i;
            if (key == it->first)
                return it;
        }
        return this->end();
    }

    pair<iterator, bool> insert(const Key& key, const T& t) {
        bool b {false};
        iterator it = find(key);
        if (it == this->end()) {
            this->push(make_pair(key, t));
            b = true;
            it = this->begin() + this->size() - 1;
        }
        return {it, b};
    }

    pair<iterator, bool> insert(const pair<Key, T>& p) {
        bool b {false};
        iterator it = find(p.first);
        if (it == this->end()) {
            this->push(p);
            b = true;
            it = this->begin() + this->size() - 1;
        }
        return {it, b};
    }

    pair<iterator, bool> emplace(const Key& key, const T& t) {
        return insert(key, t);
    }

    pair<iterator, bool> emplace(const pair<Key, T>& p) {
        return insert(p);
    }

    const T& at(const Key& key) const {
        // iterator it = find(key);
        // if (it != this->end()) return it->second;
        // return T();
        return find(key)->second;
    }

    T& at(const Key& key) {
        // iterator it = find(key);
        // if (it != this->end()) return it->second;
        // return T();
        return find(key)->second;
    }

    iterator erase(const const_iterator& it) {
        iterator i = find(it->first);
        return base::erase(i);
    }

    iterator erase(const Key& key) {
        iterator i = find(key);
        return base::erase(i);
    }

    iterator erase(const size_t index) {
        if (index < this->size()) {
            iterator it = this->begin() + index;
            erase(it);
        }
        return this->end();
    }

    T& operator[](const Key& key) {
        iterator it = find(key);
        if (it != this->end()) return it->second;

        insert(::arx::make_pair(key, T()));
        return this->back().second;
    }

private:
    using base::assign;
    using base::back;
    using base::capacity;
    using base::data;
    using base::emplace_back;
    using base::front;
    using base::pop;
    using base::pop_back;
    using base::pop_front;
    using base::push;
    using base::push_back;
    using base::push_front;
    using base::resize;
    using base::shrink_to_fit;
    using base::fill;
};

}  // namespace arx

template <typename T, size_t N>
using ArxRingBuffer = arx::RingBuffer<T, N>;

#endif  // Do not have libstdc++11
#endif  // ARX_RINGBUFFER_H
