#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
#include <algorithm>
#include <iostream>

template<typename T>

struct vector {
    typedef T *iterator;
    typedef T const *const_iterator;

    vector();                               // O(1) nothrow
    vector(vector const &);                  // O(N) strong
    vector &operator=(vector const &other); // O(N) strong

    ~vector();                              // O(N) nothrow

    T &operator[](size_t i);                // O(1) nothrow
    T const &operator[](size_t i) const;    // O(1) nothrow

    T *data();                              // O(1) nothrow
    T const *data() const;                  // O(1) nothrow
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

    iterator insert(iterator pos, T const &); // O(N) weak
    iterator insert(const_iterator pos, T const &); // O(N) weak

    iterator erase(iterator pos);           // O(N) weak
    iterator erase(const_iterator pos);     // O(N) weak

    iterator erase(iterator first, iterator last); // O(N) weak
    iterator erase(const_iterator first, const_iterator last); // O(N) weak

private:
    size_t increase_capacity();

    void push_back_realloc(T const &);

    void destroy_elements(const vector::iterator start, const vector::iterator end);

private:
    T *data_;
    size_t size_;
    size_t capacity_;
};

template<typename T>
vector<T>::vector() {
    data_ = nullptr;
    size_ = 0;
    capacity_ = 0;
}

template<typename T>
vector<T>::vector(const vector<T> &other) {
    size_ = other.size();
    capacity_ = size_;
    if (other.size() != 0) {
        data_ = static_cast<T *>(operator new(other.size() * sizeof(T)));
        size_t i = 0;
        try {
            for (; i < other.size(); i++) {
                new(data_ + i)T(other[i]);
            }
        } catch (...) {
            destroy_elements(data_, data_ + i);
            operator delete(data_);
            throw;
        }
    } else {
        data_ = nullptr;
    }
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
    destroy_elements(begin(), end());
    operator delete(data_);
}

template<typename T>
T &vector<T>::operator[](size_t i) {
    return data_[i];
}

template<typename T>
T const &vector<T>::operator[](size_t p) const {
    return data_[p];
}

template<typename T>
T *vector<T>::data() {
    return data_;
}

template<typename T>
T const *vector<T>::data() const {
    return data_;
}

template<typename T>
size_t vector<T>::size() const {
    return size_;
}

template<typename T>
T &vector<T>::front() {
    return data_[0];
}

template<typename T>
T const &vector<T>::front() const {
    return data_[0];
}

template<typename T>
T &vector<T>::back() {
    return data_[size() - 1];
}

template<typename T>
T const &vector<T>::back() const {
    return data_[size() - 1];
}

template<typename T>
void vector<T>::push_back(const T &element) {
    T copy = element;
    increase_capacity();
    new(end())T(copy);
    size_++;
}

template<typename T>
T *vector<T>::insert(T *pos, const T &element) {
    return insert(static_cast<T const *>(pos), element);
}

template<typename T>
T *vector<T>::insert(T const *pos, const T &element) {
    T copy = element;
    size_t position = pos - data_;
    if (position == size_) {
        push_back(copy);
        return data_ + size_ - 1;
    }
    increase_capacity();
    new(end())T(data_[size_ - 1]);
    size_++;
    size_t i = size_ - 2;
    while (i > position) {
        data_[i] = data_[i - 1];
        i--;
    }
    data_[position] = copy;
    return data_ + position;
}

template<typename T>
T *vector<T>::erase(T const *pos) {
    size_t retIndex = pos - data_;
    for (size_t i = pos - data_; i < size_ - 1; i++) {
        data_[i] = data_[i + 1];
    }
    data_[size_ - 1].~T();
    size_--;
    return data_ + retIndex;
}


template<typename T>
bool vector<T>::empty() const {
    return size_ == 0;
}

template<typename T>
T *vector<T>::erase(T const *first, T const *last) {
    size_t delt = last - first;
    size_t retIndex = first - data_;
    for (size_t i = first - data_; i < size_ - delt; i++) {
        data_[i] = data_[i + delt];
    }
    destroy_elements(data_ + size_ - delt, end());
    size_ -= delt;
    return data_ + retIndex;
}

template<typename T>
T *vector<T>::erase(T *pos) {
    return erase(static_cast<T const *>(pos));
}

template<typename T>
T *vector<T>::erase(T *first, T *second) {
    return erase(static_cast<T const *>(first), static_cast<T *const>(second));
}

template<typename T>
size_t vector<T>::capacity() const {
    return capacity_;
}

template<typename T>
void vector<T>::reserve(size_t new_capacity) {
    if (new_capacity > capacity()) {
        vector<T> temp;
        temp.data_ = static_cast<T *>(operator new(new_capacity * sizeof(T)));
        temp.capacity_ = new_capacity;
        for (size_t i = 0; i < size_; i++) {
            temp.push_back(data_[i]);
        }
        temp.swap(*this);
    }
}

template<typename T>
void vector<T>::pop_back() {
    data_[size_ - 1].~T();
    size_--;
}

template<typename T>
void vector<T>::shrink_to_fit() {
    if (size() == capacity()) return;
    (vector<T>(*this)).swap(*this);
}

template<typename T>
void vector<T>::clear() {
    destroy_elements(begin(), end());
    size_ = 0;
}

template<typename T>
T *vector<T>::begin() {
    return data_;
}

template<typename T>
T *vector<T>::end() {
    return data_ + size();
}

template<typename T>
T const *vector<T>::begin() const {
    return data_;
}

template<typename T>
T const *vector<T>::end() const {
    return data_ + size();
}

template<typename T>
void vector<T>::destroy_elements(const vector::iterator start, const vector::iterator end) {
    for (size_t i = 0; i < end - start; i++) {
        start[i].~T();
    }
}

template<typename T>
size_t vector<T>::increase_capacity() {
    if (size() == capacity()) {
        reserve(capacity_ == 0 ? 4 : 2 * capacity_);
    }
    return size();
}

#endif // VECTOR_H
