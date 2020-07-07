#include <cstring>
#include "optimized_vector.h"


optimized_vector::optimized_vector() : is_big(false), size_(0), buf_() {}


optimized_vector::~optimized_vector() {
    if (is_big) {
        delete buf_.dynamic_buffer;
    }
}

optimized_vector::optimized_vector(optimized_vector const &other) :
        is_big(other.is_big),
        size_(other.size_),
        buf_(other.buf_) {
    if (other.is_big) {
        buf_.dynamic_buffer = new shared_vector(*other.buf_.dynamic_buffer);
    }
}

optimized_vector::optimized_vector(std::vector<uint32_t> const &other) :
        is_big(other.size() > SMALL_SIZE),
        size_(other.size()),
        buf_() {
    if (!is_big) {
        std::copy(other.begin(), other.end(), buf_.static_buffer);
    } else {
        buf_.dynamic_buffer = new shared_vector(other);
    }
}

size_t optimized_vector::capacity() const {
    return is_big ? buf_.dynamic_buffer->capacity() : SMALL_SIZE;
}

optimized_vector &optimized_vector::operator=(optimized_vector other) {
    swap(other);
    return *this;
}

void optimized_vector::resize(size_t sz) {
    ensure_capacity(sz);
    size_ = sz;
    if (is_big) {
        buf_.dynamic_buffer->resize(sz);
    } else {
        std::fill(buf_.static_buffer + size_, buf_.static_buffer + SMALL_SIZE, 0);
    }
}


uint32_t const &optimized_vector::operator[](size_t index) const {
    return is_big ? (*buf_.dynamic_buffer)[index] : buf_.static_buffer[index];
}

uint32_t &optimized_vector::operator[](size_t index) {
    return is_big ? (*buf_.dynamic_buffer)[index] : buf_.static_buffer[index];
}

void optimized_vector::pop_back() {
    if (is_big) {
        (*buf_.dynamic_buffer).pop_back();
    }
    size_--;
}

void optimized_vector::push_back(uint32_t x) {
    ensure_capacity(size_ + 1);
    if (is_big) {
        buf_.dynamic_buffer->push_back(x);
    } else {
        buf_.static_buffer[size_] = x;
    }
    size_++;
}

size_t optimized_vector::size() const {
    return size_;
}

void optimized_vector::swap(optimized_vector &other) {
    std::swap(is_big, other.is_big);
    std::swap(size_, other.size_);
    std::swap(buf_, other.buf_);
}

void optimized_vector::ensure_capacity(size_t cap) {
    if (!is_big) {
        if (cap <= SMALL_SIZE) {
            return;
        } else {
            auto *new_data = new shared_vector;
            for (size_t j = 0; j < size_; j++) {
                new_data->push_back(buf_.static_buffer[j]);
            }
            buf_.dynamic_buffer = new_data;
            is_big = true;
        }
    }
}
