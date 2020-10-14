//
// Created by sancho20021 on 08.07.2020.
//

#ifndef VECTOR_MY_VECTOR_H
#define VECTOR_MY_VECTOR_H

#include <cstddef>
#include <algorithm>
#include <cassert>

template<typename T>

struct vector {
    using iterator = T *;
    using const_iterator = T const *;

    vector();                               // O(1) nothrow
    vector(vector const &);                  // O(N) strong
    vector &operator=(vector const &other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T &operator[](size_t i);                // O(1) nothrow
    T const &operator[](size_t i) const;    // O(1) nothrow

    iterator data();                              // O(1) nothrow
    const_iterator data() const;                  // O(1) nothrow
    size_t size() const;                    // O(1) nothrow

    T &front();                             // O(1) nothrow
    T const &front() const;                 // O(1) nothrow

    T &back();                              // O(1) nothrow
    T const &back() const;                  // O(1) nothrow
    void push_back(T const &);               // O(1)* strong
    void pop_back();                        // O(1) nothrow

    bool empty() const;                     // O(1) nothrow

    size_t capacity() const;                // O(1) nothrow
    void reserve(size_t);                   // O(N) strong
    void shrink_to_fit();                   // O(N) strong

    void clear();                           // O(N) nothrow

    void swap(vector &);                     // O(1) nothrow

    iterator begin();                       // O(1) nothrow
    iterator end();                         // O(1) nothrow

    const_iterator begin() const;           // O(1) nothrow
    const_iterator end() const;             // O(1) nothrow

    iterator insert(const_iterator pos, T const &); // O(N) weak

    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
    void increase_capacity();

    static void destroy_elements(T const *start, size_t size);

    void change_buf(size_t new_capacity);

    static T *my_alloc(size_t size);

    static T *copy_buf(size_t capacity, T *old_buf, size_t size);


private:
    T *data_;
    size_t size_;
    size_t capacity_;
};

template<typename T>
vector<T>::vector() :
        data_(nullptr),
        size_(0),
        capacity_(0) {}

template<typename T>
vector<T>::vector(const vector<T> &other) :
        size_(other.size_),
        capacity_(size_),
        data_(nullptr) {
    data_ = copy_buf(size_, other.data_, size_);
}

template<typename T>
void vector<T>::swap(vector &other) {
    std::swap(data_, other.data_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template<typename T>
vector<T> &vector<T>::operator=(vector<T> const &other) {
    vector<T>(other).swap(*this);
    return *this;
}

template<typename T>
vector<T>::~vector<T>() {
    clear();
    operator delete(data_);
}

template<typename T>
T &vector<T>::operator[](size_t i) {
    return data_[i];
}

template<typename T>
T const &vector<T>::operator[](size_t i) const {
    return data_[i];
}

template<typename T>
size_t vector<T>::size() const {
    return size_;
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator pos) {
    return erase(pos, pos + 1);
}

template<typename T>
typename vector<T>::iterator vector<T>::erase(const_iterator first, const_iterator last) {
    assert(data_ <= first && last <= data_ + size_);
    ptrdiff_t delt = last - first;
    if (delt <= 0) {
        return last - data_ + data_;
    }
    for (ptrdiff_t i = first - data_; i < size_ - delt; i++) {
        data_[i] = data_[i + delt];
    }
    destroy_elements(data_ + size_ - delt, delt);
    size_ -= delt;
    return first - data_ + data_;
}

template<typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
void vector<T>::clear() {
    destroy_elements(data_, size_);
    size_ = 0;
}

template<typename T>
void vector<T>::destroy_elements(T const *start, size_t size) {
    for (size_t i = size; i != 0; i--) {
        start[i - 1].~T();
    }
}

template<typename T>
T *vector<T>::my_alloc(size_t size) {
    return static_cast<T *>(operator new(size * sizeof(T)));
}

//O(n) strong
template<typename T>
T *vector<T>::copy_buf(size_t new_capacity, T *old_data, size_t size) {
    T *new_data = nullptr;
    if (new_capacity != 0) {
        new_data = my_alloc(new_capacity);
        size_t i = 0;
        try {
            for (; i != size; i++) {
                new(new_data + i) T(old_data[i]);
            }
        } catch (...) {
            destroy_elements(new_data, i);
            operator delete(new_data);
            throw;
        }
    }
    return new_data;
}

#endif //VECTOR_MY_VECTOR_H
